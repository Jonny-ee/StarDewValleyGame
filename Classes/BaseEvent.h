#pragma once
#include"Player.h"
#include"GameMap.h"
#include"cocos2d.h"

class BaseEvent :public cocos2d::Node {
public:
    virtual bool init() override;
    virtual void update(float dt) override;  
protected:
    // 事件触发条件检查
    virtual bool checkTriggerCondition() = 0;
    // 事件执行
    virtual void executeEvent() = 0;
    // 事件结束
    virtual void finishEvent() = 0;
    // 获取提示文本
    virtual std::string getPromptText() = 0;  

    bool _isExecuting = false;
    bool _isInTriggerArea = false;  // 是否在触发区域内
    GameMap* _gameMap = nullptr;
    Player* _player = nullptr;
    cocos2d::Label* _promptLabel = nullptr;  // 提示文本
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;  // 键盘监听器

    // 设置黑色滤镜
    void showBlackFilter(float duration = 1.0f);
    void hideBlackFilter(float duration = 1.0f);
    cocos2d::LayerColor* _blackFilter = nullptr;
    cocos2d::Size getMapSize() const;

    void showPrompt();
    void hidePrompt();
    void initKeyboardListener();
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode);
};