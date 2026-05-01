#pragma once
#include <Geode/Geode.hpp>
#include <vector>
#include <queue>

using namespace geode::prelude;

struct Action {
    float time;
    int button; // 0 = jump, 1 = hold
    bool press;
};

class BotEngine {
private:
    std::queue<Action> m_actionQueue;
    float m_currentTime;
    bool m_isRecording;
    std::vector<Action> m_recordedActions;
    
    // Obstacle detection
    float m_scanDistance = 200.0f;
    float m_reactionTime = 0.1f;
    
public:
    BotEngine() : m_currentTime(0), m_isRecording(false) {}
    
    void update(float dt, PlayLayer* playLayer);
    void reset();
    
    // Recording functions
    void startRecording();
    void stopRecording();
    void recordAction(int button, bool press);
    void playRecording();
    
    // AI functions
    bool shouldJump(PlayLayer* playLayer);
    bool detectObstacle(PlayLayer* playLayer, float distance);
    float calculateJumpTiming(float obstacleDistance, float playerSpeed);
    
    // Getters
    bool isRecording() const { return m_isRecording; }
    size_t getRecordedActionsCount() const { return m_recordedActions.size(); }
};