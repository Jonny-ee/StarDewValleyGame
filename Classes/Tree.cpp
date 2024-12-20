#include "Tree.h"

Tree* Tree::create(const std::string& spriteName, int health)
{
    Tree* tree = new (std::nothrow) Tree();
    if (tree && tree->init(spriteName, health))
    {
        tree->autorelease();
        return tree;
    }
    CC_SAFE_DELETE(tree);
    return nullptr;
}

Tree::Tree() :
    _health(100),
    _chopped(false),
    _canChop(true),
    _onTreeChopped(nullptr)
{
}

Tree::~Tree()
{
}

bool Tree::init(const std::string& spriteName, int health)
{
    if (!Sprite::initWithFile(spriteName))
    {
        return false;
    }
    _health = health;
    _chopped = false;
    _canChop = true;

    this->setAnchorPoint(Vec2(0.5f, 0.0f));
    this->setTextureRect(cocos2d::Rect(192, 160, 48, 80));
    this->setScale(3.0f);
    return true;
}

bool Tree::chop(int damage)
{
    if (!canBeChopped())
    {
        return false;
    }

    _health -= damage;

    // 使用RotateBy来实现抖动，这样会以底部为轴心抖动
    auto shakeBy = Sequence::create(
        RotateBy::create(0.05f, -5),  // 向左倾斜5度
        RotateBy::create(0.05f, 10),  // 向右倾斜10度
        RotateBy::create(0.05f, -10), // 向左倾斜10度
        RotateBy::create(0.05f, 5),   // 回到原位
        nullptr
    );
    this->runAction(shakeBy);

    if (_health <= 0)
    {
        _chopped = true;

        // 倒下动画也会以底部为轴心
        auto fallRotation = RotateTo::create(0.5f, 90.0f); // 向右倒下
        auto fadeOut = FadeOut::create(0.3f);
        auto removeTree = CallFunc::create([this]() {
            if (_onTreeChopped)
            {
                _onTreeChopped();
            }
            this->removeFromParent();
            });

        this->runAction(Sequence::create(
            fallRotation,
            fadeOut,
            removeTree,
            nullptr
        ));

        return true;
    }

    return false;
}

