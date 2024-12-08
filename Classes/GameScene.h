#pragma once
#include "cocos2d.h"
#include "Player.h"

/*
 * ��Ϸ������
 * ���ܣ�������Ϸ��������
 * 1. ��ͼ���غ���ʾ
 * 2. ��Ҷ������
 * 3. ��������
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();  // ��������
    virtual bool init();                   // ��ʼ������
    CREATE_FUNC(GameScene);
    virtual void update(float dt);         // �������º���

private:
    Player* player;                             // ��Ҷ���ָ��
    cocos2d::TMXTiledMap* tileMap;              // Tiled ��ͼ����
    cocos2d::TMXObjectGroup* collisionsGroup;   // ��ײ�������
};