#pragma once
#include "cocos2d.h"
#include "Player.h"

/*
 * 游戏场景类
 * 功能：管理游戏的主场景
 * 1. 地图加载和显示
 * 2. 玩家对象管理
 * 3. 场景更新
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();  // 创建场景
    virtual bool init();                   // 初始化场景
    CREATE_FUNC(GameScene);
    virtual void update(float dt);         // 场景更新函数

private:
    Player* player;                             // 玩家对象指针
    cocos2d::TMXTiledMap* tileMap;              // Tiled 地图对象
    cocos2d::TMXObjectGroup* collisionsGroup;   // 碰撞层对象组
};