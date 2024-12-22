#include "Marlon.h"

USING_NS_CC;

Marlon* Marlon::create()
{
    Marlon* npc = new (std::nothrow) Marlon();
    if (npc && npc->init())
    {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}

bool Marlon::init()
{
    const std::string imagePath = "Characters/Marlon.png"; // 马龙 NPC 的图片路径
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

void Marlon::initializeDefaultBehavior()
{
    setName("Malon");
    completeDialogue();
}

void Marlon::staticAnimation()
{
    ;
}
