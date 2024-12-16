#include "BaseEvent.h"
class SleepEvent : public BaseEvent {
public:
   static SleepEvent* create(GameMap* gameMap, Player* player);

protected:
    virtual bool checkTriggerCondition() override;
    virtual void executeEvent() override;
    virtual void finishEvent() override;
    virtual std::string getPromptText() override {
        return "Please enter M to sleep";
    }
private:
    const cocos2d::Vec2 TRIGGER_POS = cocos2d::Vec2(26, 6);
    const cocos2d::Vec2 SLEEP_POS = cocos2d::Vec2(25, 6);
    const cocos2d::Vec2 WAKEUP_POS = cocos2d::Vec2(23, 6);
};