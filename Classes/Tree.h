#pragma once
#include "cocos2d.h"
USING_NS_CC;

class Tree : public cocos2d::Sprite {
public:
    Tree(const std::string& spriteName, int health);
    virtual ~Tree();

    // ��ʼ����
    bool initWithSprite(const std::string& spriteName, int health);

    // �������ķ���
    void chop(int damage);

    // �Ƿ���Կ���
    bool canBeChopped() const;

    // ���Ƿ񱻿���
    bool isChopped() const;

private:
    int _health;  // ��������ֵ
    bool _chopped; // ���Ƿ񱻿���
    bool _canChop; // �Ƿ���Կ���
};

