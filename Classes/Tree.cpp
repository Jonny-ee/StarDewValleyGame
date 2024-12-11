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

    // �������ĳ�ʼ����ֵ������״̬
    _health = health;
    _chopped = false;
    _canChop = true;

    return true;
}

void Tree::chop(int damage) {
    if (_chopped) {
        return;  // ������Ѿ��������������ٿ���
    }

    _health -= damage;

    /*��������Ľ�����δʵ��
    if (_health <= 0) {
        _chopped = true;
        this->setTexture("tree_fallen.png");  // ���ÿ����������ͼ��
        CCLOG("Tree has been chopped down!");

        // ִ�е�����Ʒ���߼����������ľ�ģ�
        // this->dropItem();
    }*/
}

bool Tree::canBeChopped() const {
    return _canChop && !_chopped;
}

bool Tree::isChopped() const {
    return _chopped;
}
