#pragma once
#include "cocos2d.h"
#include <functional>

USING_NS_CC;

class Tree : public cocos2d::Sprite {
public:
    // 定义回调函数类型
    typedef std::function<void()> TreeCallback;

    // 创建树的静态方法
    static Tree* create(const std::string& spriteName, int health = 100);

    Tree();
    virtual ~Tree();

    // 初始化树
    virtual bool init(const std::string& spriteName, int health);

    // 砍伐树的方法，返回是否砍倒
    bool chop(int damage);

    // 获取当前生命值
    int getHealth() const { return _health; }

    // 是否可以砍伐
    bool canBeChopped() const { return !_chopped && _canChop; }

    // 树是否被砍倒
    bool isChopped() const { return _chopped; }

    // 创建掉落的木头
    void createWoodDrop();

    // 设置树木掉落物品的回调
    void setOnTreeChoppedCallback(const TreeCallback& callback) {
        _onTreeChopped = callback;
    }

private:
    int _health;  // 树的生命值
    bool _chopped; // 树是否被砍倒
    bool _canChop; // 是否可以砍伐
    TreeCallback _onTreeChopped; // 树被砍倒时的回调函数
};
