#pragma once
#include "NPC.h"
#include "GameMap.h"
#include "Player.h"
#include "DialogueBox.h"
#include <vector>

//刘易斯类
class Lewis : public NPC
{
public:
    static Lewis* create();
    virtual bool init();
    void moveToDirection(cocos2d::Vec2& destination, float dt);//移动到目的地
    void staticAnimation();//人物静止时的呼吸动画
    void moveAlongPath(float dt);//移动沿路径

    //void startConversation();
    std::vector<cocos2d::Vec2> path; // 移动路径

private:
    void initializeDefaultBehavior();  // 初始化刘易斯的默认行为
    void loadLewisDialogues() noexcept;         // 加载刘易斯专属对话

    
    int currentPathIndex = 0; // 当前路径索引

    enum class ActionState
    {
        IDLE,
        MOVING,
        WAITING
    };

    ActionState currentActionState = ActionState::IDLE; // 当前动作状态
    float waitTime = 2.0f; // 等待时间
    float elapsedTime = 0.0f; // 已经过的时间

    float waitTimer = 0.0f; // 等待计时器
    // 动画相关
    int currentDirection = 0;                   // 当前朝向(0:下, 1:上, 2:左, 3:右)
    int currentFrame = 0;                       // 当前帧索引
    float animationTimer = 0;                   // 动画计时器
    const float FRAME_INTERVAL = 0.3f;          // 帧间隔时间
};