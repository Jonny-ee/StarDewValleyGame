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
    this->setTextureRect(cocos2d::Rect(0, 0, 48, 48)); // ���ÿ�ʯ����������
    this->setScale(3.0f);
    return true;
}

void Ore::dig(int damage)
{
    if (!canBeDug())
    {
        return; // ��������ھ�ֱ�ӷ���
    }

    _health -= damage;

    // ʹ��Shake��ʵ�ֶ���Ч��
    auto shakeBy = Sequence::create(
        RotateBy::create(0.05f, -5),  // ������б5��
        RotateBy::create(0.05f, 10),   // ������б10��
        RotateBy::create(0.05f, -10),  // ������б10��
        RotateBy::create(0.05f, 5),    // �ص�ԭλ
        nullptr
    );
    this->runAction(shakeBy);

    if (_health <= 0)
    {
        _dug = true;

        // ��ʯ���ھ��Ķ���
        auto fadeOut = FadeOut::create(0.3f);
        auto removeOre = CallFunc::create([this]() {
            if (_onOreDug)
            {
                _onOreDug(); // �����ھ��Ļص�
            }
            this->removeFromParent(); // �Ƴ���ʯ
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