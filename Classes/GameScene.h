#pragma once
#include "cocos2d.h"
#include "Player.h"

/*
 * 游戏场景类
 * 功能：管理游戏主场景，包括：
 * 1.地图加载和显示
 * 2.玩家角色管理
 * 3.碰撞检测
 * 4.键盘输入处理
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    // 游戏对象
    Player* player;                             // 玩家角色指针
    cocos2d::TMXTiledMap* tileMap;              // Tiled 地图对象
    cocos2d::TMXObjectGroup* collisionsGroup;   // 碰撞层组对象

    // 游戏功能方法
    bool checkCollision(const cocos2d::Vec2& nextPosition);                                         // 碰撞检测
    bool isPointInPolygon(const cocos2d::Vec2& point, const std::vector<cocos2d::Vec2>& vertices);  // 判断点是否在多边形内的辅助函数
    void update(float dt);                                                                          // 场景更新
    void initKeyboardListener();                                                                    // 初始化键盘监听
    bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode);                                     // 检查按键状态

    // 键盘状态存储
    std::map<cocos2d::EventKeyboard::KeyCode, bool> keys;       // 存储按键状态的映射表
};