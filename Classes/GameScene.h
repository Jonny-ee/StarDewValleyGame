#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"
#include <set>

/*
 * ��Ϸ������
 * ���ܣ�������Ϸ������
 * 1. ��ͼ���غ���ʾ
 * 2. ��Ҷ������
 * 3. �����л�
 * 4. ���봦��
 * 5. �������
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();  // ��������
    virtual bool init();                   // ��ʼ������
    CREATE_FUNC(GameScene);
    virtual void update(float dt);         // �������º���

    // �����л����
    void switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);
    void switchToFarmMap();

    // �������
    void updateCamera();

    // ���봦��
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

private:
    // ��Ϸ����
    Player* player = nullptr;                             // ��Ҷ���ָ��
    GameMap* _gameMap = nullptr;                         // �µ�ͼϵͳ
    cocos2d::TMXTiledMap* tileMap = nullptr;            // ��ͳ��ͼϵͳ
    cocos2d::TMXObjectGroup* collisionsGroup = nullptr;  // ��ײ��

    // ����״̬
    std::set<char> _pressedKeys;                         // �Ѱ��µİ�������
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;  // ���̼�����
};