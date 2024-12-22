#pragma once
#include "BaseEvent.h"

class Cooking : public BaseEvent {
public:
    // 创建烹饪事件实例
    static Cooking* create(GameMap* gameMap, Player* player);
protected:
    // 检查烹饪事件触发条件
    virtual bool checkTriggerCondition() override;
    // 执行烹饪事件
    virtual void executeEvent() override;
    // 完成烹饪事件
    virtual void finishEvent() override;
    // 获取提示文本
    virtual std::string getPromptText() override {
        return "Please enter M to start cooking";
    }
private:
    // 烹饪触发位置（厨房位置）
    const cocos2d::Vec2 TRIGGER_POS = cocos2d::Vec2(10, 5);
    // 提示标签
    cocos2d::Label* tipLabel = nullptr;
    // 隐藏提示信息
    void hideTip();
    // 显示提示信息及其持续时间
    void showTip(const std::string& text, float duration);
};
