#include "Alex.h"
USING_NS_CC;
Alex* Alex::create()
{
    Alex* alex = new (std::nothrow) Alex();
    if (alex && alex->init()) {
        alex->autorelease();
        return alex;
    }
    CC_SAFE_DELETE(alex);
    return nullptr;
}
bool Alex::init()
{
    const std::string imagePath = "Characters/Alex.png";    // 艾利克斯 NPC 的图片路径
    if (!Sprite::initWithFile(imagePath)) { 
        return false;
    }
    //初始化基本信息
    moveSpeed = 50.0f;                                      //移速
    heartPoint = 0;
    relationship = Relation::DEFAULT;
    //设置精灵属性
    this->setAnchorPoint(Vec2(0.5f, 0.5f));                 //设置锚点
    this->setScale(2.0f);                                   //设置缩放
    this->setTextureRect(cocos2d::Rect(0, 128, 16, 32));    // 设置尺寸
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));        // 设置锚点
    this->setScale(2.5);
    initializeDefaultBehavior();
    return true;
}
void Alex::initializeDefaultBehavior()
{
    setName("Alex");
    completeDialogue();
}
void Alex::staticAnimation()
{
    ;
}
void Alex::initializeAnimations()
{
    // 创建动画
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.15f);                      // 每帧持续时间
    // 精灵表有8帧动画
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 4; j++) {
            auto frame = SpriteFrame::create("Characters/Alex.png", cocos2d::Rect(j * 16, 128 + i * 32, 16, 32)); // 每帧的区域
            animation->addSpriteFrame(frame);
        }
    }
    // 创建循环动画
    auto runAnimation = RepeatForever::create(Animate::create(animation));
    this->runAction(runAnimation);                          // 运行动画
}
