#include "Ore.h"

USING_NS_CC;

Ore* Ore::create(const std::string& spriteName, int health)
{
    Ore* ore = new (std::nothrow) Ore();
    if (ore && ore->init(spriteName, health))
    {
        ore->autorelease();
        return ore;
    }
    CC_SAFE_DELETE(ore);
    return nullptr;
}

Ore::Ore() :
    _health(100),
    _dug(false),
    _canDug(true),
    _onOreDug(nullptr)
{
}

Ore::~Ore()
{
}

bool Ore::init(const std::string& spriteName, int health)
{
    if (!Sprite::initWithFile(spriteName))
    {
        return false;
    }
    _health = health;
    _dug = false;
    _canDug = true;

    this->setAnchorPoint(Vec2(0.5f, 0.0f));
    this->setTextureRect(cocos2d::Rect(0, 0, 48, 48)); // 设置矿石的纹理区域
    this->setScale(3.0f);
    return true;
}

void Ore::dig(int damage)
{
    if (!canBeDug())
    {
        return; // 如果不能挖掘，直接返回
    }

    _health -= damage;

    // 使用Shake来实现抖动效果
    auto shakeBy = Sequence::create(
        RotateBy::create(0.05f, -5),  // 向左倾斜5度
        RotateBy::create(0.05f, 10),   // 向右倾斜10度
        RotateBy::create(0.05f, -10),  // 向左倾斜10度
        RotateBy::create(0.05f, 5),    // 回到原位
        nullptr
    );
    this->runAction(shakeBy);

    if (_health <= 0)
    {
        _dug = true;

        // 矿石被挖掘后的动画
        auto fadeOut = FadeOut::create(0.3f);
        auto removeOre = CallFunc::create([this]() {
            if (_onOreDug)
            {
                _onOreDug(); // 调用挖掘后的回调
            }
            this->removeFromParent(); // 移除矿石
            });

        this->runAction(Sequence::create(
            fadeOut,
            removeOre,
            nullptr
        ));
    }
}

void Ore::remove()
{
    this->removeFromParentAndCleanup(true);
}