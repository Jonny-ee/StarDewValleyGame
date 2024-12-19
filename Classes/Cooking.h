#pragma once
#include "BaseEvent.h"
class Cooking : public BaseEvent {
public:
    static Cooking* create(GameMap* gameMap, Player* player);

protected:
    virtual bool checkTriggerCondition() override;
    virtual void executeEvent() override;
    virtual void finishEvent() override;
    virtual std::string getPromptText() override {
        return "Please enter M to start cooking";
    }
private:
    const cocos2d::Vec2 TRIGGER_POS = cocos2d::Vec2(10, 5);
    cocos2d::Label* tipLabel = nullptr;
    void hideTip();
    void showTip(const std::string& text, float duration);
};
