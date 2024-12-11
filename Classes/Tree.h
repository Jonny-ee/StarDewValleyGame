#pragma once
#include "cocos2d.h"
USING_NS_CC;

class Tree : public cocos2d::Sprite {
public:
    Tree(const std::string& spriteName, int health);
    virtual ~Tree();

    // 初始化树
    bool initWithSprite(const std::string& spriteName, int health);

    // 砍伐树的方法
    void chop(int damage);

    // 是否可以砍伐
    bool canBeChopped() const;

    // 树是否被砍倒
    bool isChopped() const;

private:
    int _health;  // 树的生命值
    bool _chopped; // 树是否被砍倒
    bool _canChop; // 是否可以砍伐
};

