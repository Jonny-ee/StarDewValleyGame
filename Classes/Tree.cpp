#include "Tree.h"

USING_NS_CC;

/*
 * ��ľ���캯��
 * @param spriteName ��ľ����ͼƬ����
 * @param health ��ľ������ֵ
 */
Tree::Tree(const std::string& spriteName, int health)
    : _health(health), _chopped(false), _canChop(true) {
    this->initWithSprite(spriteName, health);
}

// ��������
Tree::~Tree() {}

/*
 * ��ʼ����ľ����
 * @param spriteName ����ͼƬ����
 * @param health ��ʼ����ֵ
 * @return ��ʼ���Ƿ�ɹ�
 */
bool Tree::initWithSprite(const std::string& spriteName, int health) {
    if (!Sprite::initWithFile(spriteName)) {
        return false;
    }

    // ������ľ�ĳ�ʼ����ֵ������״̬
    _health = health;
    _chopped = false;
    _canChop = true;

    return true;
}

/*
 * ������ľ
 * @param damage ��ɵ��˺�ֵ
 * ���ܣ�������ľ������ֵ������������߼�
 */
void Tree::chop(int damage) {
    if (_chopped) {
        return;  // ����Ѿ������������ٿɿ�
    }

    _health -= damage;

    /* ������ĵ�����δʵ��
    if (_health <= 0) {
        _chopped = true;
        this->setTexture("tree_fallen.png");  // ���ÿ��������ľͼ��
        CCLOG("Tree has been chopped down!");

        // ִ�е�����Ʒ���߼��������ľ�ģ�
        // this->dropItem();
    }*/
}

/*
 * ����Ƿ���Կ���
 * @return ������ľ�Ƿ���Ա�����
 */
bool Tree::canBeChopped() const {
    return _canChop && !_chopped;
}

/*
 * ����Ƿ��ѱ�����
 * @return ������ľ�Ƿ��ѱ�����
 */
bool Tree::isChopped() const {
    return _chopped;
}