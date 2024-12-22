#pragma once
#include "cocos2d.h"

class Ore : public cocos2d::Sprite
{
public:
    static Ore* create(const std::string& spriteName, int health);
    bool canBeDug() const { return _canDug; } // 矿石可以被挖掘
    void dig(int damage); // 挖掘矿石
    void remove(); // 移除矿石

    // 设置挖掘后回调
    void setOnOreDug(const std::function<void()>& callback) { _onOreDug = callback; }

private:
    Ore(); // 构造函数
    ~Ore(); // 析构函数
    bool init(const std::string& spriteName, int health); // 初始化
    int _health; // 矿石的生命值
    bool _dug; // 矿石是否被挖掘
    bool _canDug; // 矿石是否可以被挖掘
    std::function<void()> _onOreDug; // 挖掘后的回调
};