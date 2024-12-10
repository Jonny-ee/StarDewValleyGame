#pragma once
#include "cocos2d.h"

/*
 * 玩家类
 * 功能：管理玩家角色的行为和属性
 */
class Player : public cocos2d::Sprite
{
public:

    static Player* create();                        // 创建玩家对象
    virtual bool init();                            // 初始化玩家对象
    void moveInDirection(cocos2d::Vec2 direction);  // 移动玩家
    float getMoveSpeed() const;                     // 获取玩家移动速度

private:
    float moveSpeed = 200.0f;                       // 玩家移动速度（可调）
};