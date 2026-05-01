#include <Geode/Geode.hpp>
#include <Geode/modify/UILayer.hpp>
#include "BotEngine.hpp"

using namespace geode::prelude;

extern BotEngine* g_botEngine;

class $modify(BotUILayer, UILayer) {
    bool init(PlayLayer* playLayer) {
        if (!UILayer::init(playLayer)) {
            return false;
        }
        
        // Add bot control buttons
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        
        // Record button
        auto recordBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_button_01.png"),
            this,
            menu_selector(BotUILayer::onRecordToggle)
        );
        recordBtn->setPosition({50, 50});
        menu->addChild(recordBtn);
        
        // Play recording button
        auto playBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_button_02.png"),
            this,
            menu_selector(BotUILayer::onPlayRecording)
        );
        playBtn->setPosition({50, 100});
        menu->addChild(playBtn);
        
        this->addChild(menu);
        
        return true;
    }
    
    void onRecordToggle(CCObject* sender) {
        if (!g_botEngine) return;
        
        if (g_botEngine->isRecording()) {
            g_botEngine->stopRecording();
            Notification::create("Recording stopped", NotificationIcon::Success)->show();
        } else {
            g_botEngine->startRecording();
            Notification::create("Recording started", NotificationIcon::Info)->show();
        }
    }
    
    void onPlayRecording(CCObject* sender) {
        if (!g_botEngine) return;
        
        g_botEngine->playRecording();
        Notification::create("Playing recording", NotificationIcon::Success)->show();
    }
};