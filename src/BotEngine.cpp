#include "BotEngine.hpp"

void BotEngine::update(float dt, PlayLayer* playLayer) {
    m_currentTime += dt;
    
    // Process queued actions
    while (!m_actionQueue.empty()) {
        auto& action = m_actionQueue.front();
        if (action.time <= m_currentTime) {
            playLayer->pushButton(action.button, action.press);
            m_actionQueue.pop();
        } else {
            break;
        }
    }
}

void BotEngine::reset() {
    while (!m_actionQueue.empty()) {
        m_actionQueue.pop();
    }
    m_currentTime = 0;
}

void BotEngine::startRecording() {
    m_isRecording = true;
    m_recordedActions.clear();
    m_currentTime = 0;
}

void BotEngine::stopRecording() {
    m_isRecording = false;
    log::info("Recorded {} actions", m_recordedActions.size());
}

void BotEngine::recordAction(int button, bool press) {
    if (m_isRecording) {
        m_recordedActions.push_back({m_currentTime, button, press});
    }
}

void BotEngine::playRecording() {
    reset();
    for (const auto& action : m_recordedActions) {
        m_actionQueue.push(action);
    }
}

bool BotEngine::shouldJump(PlayLayer* playLayer) {
    auto player = playLayer->m_player1;
    if (!player) return false;
    
    // Check if player is on ground
    bool onGround = player->m_isOnGround;
    if (!onGround) return false;
    
    // Detect obstacles ahead
    return detectObstacle(playLayer, m_scanDistance);
}

bool BotEngine::detectObstacle(PlayLayer* playLayer, float distance) {
    auto player = playLayer->m_player1;
    if (!player) return false;
    
    float playerX = player->getPositionX();
    float playerY = player->getPositionY();
    float playerSpeed = playLayer->m_gameState.m_currentSpeed;
    
    // Scan for objects in front of player
    auto objects = playLayer->m_objects;
    
    for (auto* obj : CCArrayExt<GameObject*>(objects)) {
        if (!obj) continue;
        
        float objX = obj->getPositionX();
        float objY = obj->getPositionY();
        
        // Check if object is ahead and within scan distance
        if (objX > playerX && objX < playerX + distance) {
            // Check if object is at player height (potential collision)
            float heightDiff = std::abs(objY - playerY);
            if (heightDiff < 50.0f) {
                // Check if it's a hazard
                if (obj->m_objectType == GameObjectType::Hazard ||
                    obj->m_objectType == GameObjectType::Solid) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

float BotEngine::calculateJumpTiming(float obstacleDistance, float playerSpeed) {
    // Calculate when to jump based on distance and speed
    float timeToObstacle = obstacleDistance / playerSpeed;
    return timeToObstacle - m_reactionTime;
}