#include "Tree.h"

USING_NS_CC;

/*
 * 树木构造函数
 * @param spriteName 树木精灵图片名称
 * @param health 树木的生命值
 */
Tree::Tree(const std::string& spriteName, int health)
    : _health(health), _chopped(false), _canChop(true) {
    this->initWithSprite(spriteName, health);
}

// 析构函数
Tree::~Tree() {}

/*
 * 初始化树木精灵
 * @param spriteName 精灵图片名称
 * @param health 初始生命值
 * @return 初始化是否成功
 */
bool Tree::initWithSprite(const std::string& spriteName, int health) {
    if (!Sprite::initWithFile(spriteName)) {
        return false;
    }

    // 设置树木的初始生命值和其他状态
    _health = health;
    _chopped = false;
    _canChop = true;

    return true;
}

/*
 * 砍伐树木
 * @param damage 造成的伤害值
 * 功能：减少树木的生命值，处理砍伐相关逻辑
 */
void Tree::chop(int damage) {
    if (_chopped) {
        return;  // 如果已经被砍倒，则不再可砍
    }

    _health -= damage;

    /* 砍倒后的掉落物未实现
    if (_health <= 0) {
        _chopped = true;
        this->setTexture("tree_fallen.png");  // 设置砍倒后的树木图标
        CCLOG("Tree has been chopped down!");

        // 执行掉落物品的逻辑（如掉落木材）
        // this->dropItem();
    }*/
}

/*
 * 检查是否可以砍伐
 * @return 返回树木是否可以被砍伐
 */
bool Tree::canBeChopped() const {
    return _canChop && !_chopped;
}

/*
 * 检查是否已被砍倒
 * @return 返回树木是否已被砍倒
 */
bool Tree::isChopped() const {
    return _chopped;
}