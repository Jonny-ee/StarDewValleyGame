#include "Lewis.h"
Lewis* Lewis::create()
{
    Lewis* lewis = new (std::nothrow) Lewis();
    if (lewis && lewis->init()) {
        lewis->autorelease();
        return lewis;
    }
    CC_SAFE_DELETE(lewis);
    return nullptr;
}

bool Lewis::init()
{
    const std::string imagePath = "Characters/Lewis.png";
    if (!NPC::init(imagePath)) {
        return false;
    }
    this->setTextureRect(cocos2d::Rect(0, 0, 16, 32)); // 设置尺寸
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // 设置锚点
    this->setScale(2.5);
    initializeDefaultBehavior();
    loadLewisDialogues();
    return true;
}
// 设置基本信息
void Lewis::initializeDefaultBehavior()
{
    setName("Lewis");
    setOccupation("镇长");
    setBirthday(Season::SPRING, 7);
}
// 添加刘易斯专属对话
void Lewis::loadLewisDialogues() noexcept
{
    // 添加刘易斯专属对话
    // addDialogue(0, " 欢迎来到鹈鹕镇！");
    // addDialogue(0, " 作为镇长，我一直在努力让这个小镇变得更好。");
    // addDialogue(5, " 你为鹈鹕镇做出了很大贡献！");
    // addDialogue(10, " 你已经成为了这个小镇不可或缺的一部分。");
}

void Lewis::moveToDirection(cocos2d::Vec2& destination, float dt)
{
    cocos2d::Vec2 direction = destination - this->getPosition();
    float distance = direction.length();
    // 如果NPC已经到达目标位置，停止移动
    if (distance < 1.0f) {
        staticAnimation();
        return;
    }

    // 更新方向
    if (direction.y > 0)
        currentDirection = 2;
    if (direction.y < 0)
        currentDirection = 0;
    if (direction.x < 0)
        currentDirection = 3;
    if (direction.x > 0)
        currentDirection = 1;

    // 归一化方向向量
    direction.normalize();
    // 更新位置
    this->setPosition(this->getPosition() + direction * moveSpeed * dt);
    // 更新走路动画
    animationTimer += dt;
    if (animationTimer >= FRAME_INTERVAL) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % 4;
    }
    // 设置走路帧
    this->setTextureRect(cocos2d::Rect(currentFrame * 16, currentDirection * 32, 16, 32));

}void Lewis::staticAnimation()
{
    this->stopAllActions();
    this->setTextureRect(cocos2d::Rect(0, 0, 16, 32));
}

void Lewis::moveAlongPath(float dt)
{
    if (currentPathIndex >= path.size()) {
        currentPathIndex = 0;// 如果到达路径末尾，重置索引
    }

    cocos2d::Vec2 targetPosition = path[currentPathIndex];
    currentActionState = ActionState::MOVING;
    moveToDirection(targetPosition, dt);// 调用移动方法

    // 检测是否到达目标位置
    if (this->getPosition().distance(targetPosition) < 1.0f) {
        staticAnimation();// 播放静止动画
        currentPathIndex++;// 移动到下一个目标
        waitTimer = 0;// 重置计时器
    }
}