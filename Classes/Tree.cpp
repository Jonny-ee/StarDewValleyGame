#include "Tree.h"

USING_NS_CC;

Tree::Tree(const std::string& spriteName, int health)
    : _health(health), _chopped(false), _canChop(true) {
    this->initWithSprite(spriteName, health);
}

Tree::~Tree() {}

bool Tree::initWithSprite(const std::string& spriteName, int health) {
    if (!Sprite::initWithFile(spriteName)) {
        return false;
    }

    // 设置树的初始健康值和其他状态
    _health = health;
    _chopped = false;
    _canChop = true;

    return true;
}

void Tree::chop(int damage) {
    if (_chopped) {
        return;  // 如果树已经被砍倒，不能再砍了
    }

    _health -= damage;

    /*树砍倒后的交互暂未实现
    if (_health <= 0) {
        _chopped = true;
        this->setTexture("tree_fallen.png");  // 设置砍倒后的树的图像
        CCLOG("Tree has been chopped down!");

        // 执行掉落物品的逻辑（例如掉落木材）
        // this->dropItem();
    }*/
}

bool Tree::canBeChopped() const {
    return _canChop && !_chopped;
}

bool Tree::isChopped() const {
    return _chopped;
}
