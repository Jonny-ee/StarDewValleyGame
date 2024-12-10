#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"
#include <set>

/*
 * 游戏场景类
 * 功能：管理游戏主场景
 * 1. 地图加载和显示
 * 2. 玩家对象管理
 * 3. 场景切换
 * 4. 输入处理
 * 5. 相机控制
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();  // 创建场景
    virtual bool init();                   // 初始化场景
    CREATE_FUNC(GameScene);
    virtual void update(float dt);         // 场景更新函数

    // 场景切换相关
    void switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);
    void switchToFarmMap();

    // 相机控制
    void updateCamera();

    // 输入处理
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

private:
    // 游戏对象
    Player* player = nullptr;                             // 玩家对象指针
    GameMap* _gameMap = nullptr;                         // 新地图系统
    cocos2d::TMXTiledMap* tileMap = nullptr;            // 传统地图系统
    cocos2d::TMXObjectGroup* collisionsGroup = nullptr;  // 碰撞组

    // 输入状态
    std::set<char> _pressedKeys;                         // 已按下的按键集合
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;  // 键盘监听器
};