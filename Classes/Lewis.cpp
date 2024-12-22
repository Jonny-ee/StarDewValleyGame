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
    if (!Sprite::initWithFile(imagePath)) { // 直接使用 Sprite 的初始化
        return false;
    }
    //初始化基本信息
    moveSpeed = 50.0f;//移速
    heartPoint = 0;
    relationship = Relation::DEFAULT;

    //设置精灵属性
    this->setAnchorPoint(Vec2(0.5f, 0.5f));//设置锚点
    this->setScale(2.0f);//设置缩放


    this->setTextureRect(cocos2d::Rect(0, 0, 16, 32)); // 设置尺寸
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // 设置锚点
    this->setScale(2.5);
    initializeDefaultBehavior();
    return true;
}
// 设置基本信息
void Lewis::initializeDefaultBehavior()
{
    setName("Lewis");
    completeDialogue();
}

void Lewis::moveToDirection(cocos2d::Vec2& destination, float dt)
{
    cocos2d::Vec2 direction = destination - this->getPosition();
    float distance = direction.length();

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

}

void Lewis::staticAnimation()
{
    currentActionState = ActionState::IDLE; // 设置为静止状态
    this->stopAllActions(); // 停止所有动作

    // 创建两帧的动画
    auto frame1 = SpriteFrame::create("Characters/Lewis.png", cocos2d::Rect(0, 0, 16, 32)); // 第一帧
    auto frame2 = SpriteFrame::create("Characters/Lewis.png", cocos2d::Rect(32, 128, 16, 32)); // 第二帧

    // 创建动画帧数组
    Vector<SpriteFrame*> frames;
    frames.pushBack(frame1);
    frames.pushBack(frame2);

    // 创建动画
    auto animation = Animation::createWithSpriteFrames(frames, 0.5f); // 每帧持续0.5秒
    auto animate = Animate::create(animation);

    // 创建重复动作，重复6次（3秒内显示两帧）
    auto repeatAction = Repeat::create(animate, 6); // 6次，每次0.5秒，总共3秒

    // 创建延迟动作，3秒后恢复移动状态
    auto resetAction = CallFunc::create([this]() {
        this->setActionState(ActionState::MOVING); // 恢复移动状态
        });

    // 创建序列动作
    auto sequence = Sequence::create(repeatAction, resetAction, nullptr);
    this->runAction(sequence); // 运行动作序列
}

void Lewis::moveAlongPath(float dt)
{
    if (currentActionState != ActionState::MOVING) {
        return;
    }
    if (currentPathIndex >= path.size()) {
        currentPathIndex = 0;// 如果到达路径末尾，重置索引
    }

    cocos2d::Vec2 targetPosition = path[currentPathIndex];
    currentActionState = ActionState::MOVING;
    moveToDirection(targetPosition, dt);// 调用移动方法

    // 检测是否到达目标位置
    if (this->getPosition().distance(targetPosition) < 1.0f) {
        //staticAnimation();// 播放静止动画
        currentPathIndex++;// 移动到下一个目标
        waitTimer = 0;// 重置计时器
    }
}

void Lewis::showThanks()
{
    this->setTextureRect(cocos2d::Rect(32, 196, 16, 27));
    currentActionState = ActionState::IDLE;

    // 设置初始状态
    this->setScale(2.5f); // 恢复到正常大小
    this->setOpacity(255); // 确保完全不透明

    // 创建缩放和透明度变化的动作
    auto scaleUp = ScaleTo::create(0.2f, 3.0f); // 放大到3.0倍
    auto fadeOut = FadeOut::create(0.2f); // 逐渐透明
    auto fadeIn = FadeIn::create(0.2f); // 逐渐不透明
    auto scaleDown = ScaleTo::create(0.2f, 2.5f); // 缩小回2.5倍

    // 创建序列动作
    auto sequence = Sequence::create(scaleUp, fadeOut, fadeIn, scaleDown, nullptr);
    // 创建延迟动作，1.5秒后恢复状态
    auto delay = DelayTime::create(1.5f);
    auto resetAction = CallFunc::create([this]() {
        this->setActionState(ActionState::MOVING);
        });

    // 将序列动作和延迟动作结合
    auto finalSequence = Sequence::create(sequence, delay, resetAction, nullptr);
    this->runAction(finalSequence); // 运行动画

    // 好感度增加3
    this->heartPoint += 3;

}
