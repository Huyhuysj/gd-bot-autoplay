#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "BotEngine.hpp"

using namespace geode::prelude;

// Global bot engine instance
static BotEngine* g_botEngine = nullptr;

class $modify(BotPlayLayer, PlayLayer) {
    struct Fields {
        bool m_botEnabled;
        float m_playSpeed;
        BotEngine* m_botEngine;
    };
    
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }
        
        // Initialize bot engine
        if (!g_botEngine) {
            g_botEngine = new BotEngine();
        }
        m_fields->m_botEngine = g_botEngine;
        
        // Load settings
        m_fields->m_botEnabled = Mod::get()->getSettingValue<bool>("enabled");
        m_fields->m_playSpeed = Mod::get()->getSettingValue<double>("speed");
        
        log::info("Bot Auto Play initialized - Enabled: {}, Speed: {}", 
                  m_fields->m_botEnabled, m_fields->m_playSpeed);
        
        return true;
    }
    
    void update(float dt) {
        // Apply speed multiplier
        float adjustedDt = dt * m_fields->m_playSpeed;
        PlayLayer::update(adjustedDt);
        
        if (m_fields->m_botEnabled && !m_isDead && m_fields->m_botEngine) {
            m_fields->m_botEngine->update(adjustedDt, this);
            
            // AI decision making
            if (m_fields->m_botEngine->shouldJump(this)) {
                pushButton(0, true);
                // Release after short delay
                Loader::get()->queueInMainThread([this]() {
                    pushButton(0, false);
                });
            }
        }
    }
    
    void resetLevel() {
        PlayLayer::resetLevel();
        if (m_fields->m_botEngine) {
            m_fields->m_botEngine->reset();
        }
    }
    
    void levelComplete() {
        PlayLayer::levelComplete();
        log::info("Level completed with bot! Actions: {}", 
                  m_fields->m_botEngine ? m_fields->m_botEngine->getRecordedActionsCount() : 0);
    }
    
    void onQuit() {
        PlayLayer::onQuit();
        if (m_fields->m_botEngine) {
            m_fields->m_botEngine->reset();
        }
    }
};

// Hook player input for recording
class $modify(BotPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        PlayerObject::pushButton(button);
        
        if (g_botEngine && g_botEngine->isRecording()) {
            g_botEngine->recordAction(static_cast<int>(button), true);
        }
    }
    
    void releaseButton(PlayerButton button) {
        PlayerObject::releaseButton(button);
        
        if (g_botEngine && g_botEngine->isRecording()) {
            g_botEngine->recordAction(static_cast<int>(button), false);
        }
    }
};