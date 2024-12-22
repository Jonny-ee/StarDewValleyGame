#pragma once
#include "cocos2d.h"

class Ore : public cocos2d::Sprite
{
public:
    static Ore* create(const std::string& spriteName, int health);
    bool canBeDug() const { return _canDug; } // ��ʯ���Ա��ھ�
    void dig(int damage); // �ھ��ʯ
    void remove(); // �Ƴ���ʯ

    // �����ھ��ص�
    void setOnOreDug(const std::function<void()>& callback) { _onOreDug = callback; }

private:
    Ore(); // ���캯��
    ~Ore(); // ��������
    bool init(const std::string& spriteName, int health); // ��ʼ��
    int _health; // ��ʯ������ֵ
    bool _dug; // ��ʯ�Ƿ��ھ�
    bool _canDug; // ��ʯ�Ƿ���Ա��ھ�
    std::function<void()> _onOreDug; // �ھ��Ļص�
};