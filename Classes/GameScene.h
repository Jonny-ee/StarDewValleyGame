#pragma once
#include "cocos2d.h"
#include "Player.h"

/*
 * ��Ϸ������
 * ���ܣ�������Ϸ��������������
 * 1.��ͼ���غ���ʾ
 * 2.��ҽ�ɫ����
 * 3.��ײ���
 * 4.�������봦��
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    // ��Ϸ����
    Player* player;                             // ��ҽ�ɫָ��
    cocos2d::TMXTiledMap* tileMap;              // Tiled ��ͼ����
    cocos2d::TMXObjectGroup* collisionsGroup;   // ��ײ�������

    // ��Ϸ���ܷ���
    bool checkCollision(const cocos2d::Vec2& nextPosition);                                         // ��ײ���
    bool isPointInPolygon(const cocos2d::Vec2& point, const std::vector<cocos2d::Vec2>& vertices);  // �жϵ��Ƿ��ڶ�����ڵĸ�������
    void update(float dt);                                                                          // ��������
    void initKeyboardListener();                                                                    // ��ʼ�����̼���
    bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode);                                     // ��鰴��״̬

    // ����״̬�洢
    std::map<cocos2d::EventKeyboard::KeyCode, bool> keys;       // �洢����״̬��ӳ���
};