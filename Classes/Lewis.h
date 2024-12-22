#pragma once
#include "NPC.h"
#include "GameMap.h"
#include "Player.h"
#include "DialogueBox.h"
#include "QuestSystem.h"
#include <vector>

//刘易斯类
class Lewis : public NPC
{
public:
    static Lewis* create();
    virtual bool init() override;
    void moveToDirection(cocos2d::Vec2& destination, float dt);//移动到目的地
    void staticAnimation() override;//人物静止时的呼吸动
    void moveAlongPath(float dt);//移动沿路径
    std::vector<cocos2d::Vec2> path; // 移动路径
    void showThanks();//感谢动画
    void setActionState(ActionState state) { currentActionState = state; }

private:
    void initializeDefaultBehavior();  // 初始化刘易斯的默认行为
    void completeDialogue()
    {
        this->addDialogue("Welcome to Pelican Town!");
        this->addDialogue("As a mayor, I have been working hard to make this town a better place.");
        this->addDialogue("You have made great contributions to Peican Town!");
        this->addDialogue("You have become an indispensable part of this small town!");
    }
    std::function<void(const std::string&, const std::string&, const std::string&, int)> friendshipCallback; // 友谊回调

    int currentPathIndex = 0; // 当前路径索引
    ActionState currentActionState = ActionState::MOVING; // 当前动作状态
    float waitTime = 2.0f; // 等待时间
    float elapsedTime = 0.0f; // 已经过的时间

    float waitTimer = 0.0f; // 等待计时器
    // 动画相关
    int currentDirection = 0;                   // 当前朝向(0:下, 1:上, 2:左, 3:右)
    int currentFrame = 0;                       // 当前帧索引
    float animationTimer = 0;                   // 动画计时器
    const float FRAME_INTERVAL = 0.3f;          // 帧间隔时间
};
