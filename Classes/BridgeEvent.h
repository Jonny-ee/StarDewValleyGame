#pragma once
#include"BaseEvent.h"
class BridgeEvent : public BaseEvent {
public:
   static BridgeEvent* create(GameMap* gameMap, Player* player);
   virtual void update(float dt) override;

protected:
    virtual bool checkTriggerCondition() override;
    virtual void executeEvent() override;
    virtual void finishEvent() override;
    virtual std::string getPromptText() override {
        return "Please enter M to repair this bridge";
    }
private:
    bool _bridgeRepaired = false;
    const cocos2d::Vec2 TRIGGER_POS1 = cocos2d::Vec2(5, 26);
    const cocos2d::Vec2 TRIGGER_POS2 = cocos2d::Vec2(5, 27);
    const std::string TRIGGER_MAP = "Mountain";
};