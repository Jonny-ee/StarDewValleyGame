#pragma once
#include "cocos2d.h"
#include "Player.h"
#include <string>
#include <map>
#include <vector>

/* NPC类 */
class NPC :public cocos2d::Sprite // 继承自精灵类
{
public:
    // 状态
    enum class ActionState
    {
        IDLE,
        MOVING,
    };

    // 关系
    enum class Relation
    {
        DEFAULT,
        FRIEND,
        LOVER
    };
    //设置基本信息
    void setName(const std::string& name) { npcName = name; }
    virtual ~NPC() {}
    virtual void staticAnimation() = 0; // 纯虚函数

    // 获取随机对话
    void addDialogue(std::string dialogue);
    std::string getRandomDialogue();

    //好感度系统
    int getHeartPoints() const { return heartPoint > 10 ? 10 : heartPoint; } // 获取当前好感度

    //动画相关
    void showFull();

    //好感度
    int heartPoint;

protected:
    //基本信息
    std::string npcName;//姓名
    std::vector<std::string> dialogueList; // 对话列表

    Relation relationship;

    //移动速度
    float moveSpeed;

    ActionState currentActionState; // 当前动作状态
};
