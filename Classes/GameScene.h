#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"
#include <set>

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(GameScene);

    // 更新函数
    virtual void update(float dt);
    void switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);
    // 键盘事件处理函数
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void switchToFarmMap();
    //相机
    void updateCamera();

private:
    Player* player = nullptr;
    GameMap* _gameMap = nullptr;
    std::set<char> _pressedKeys;
    // 添加成员变量来跟踪监听器状态
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;
};