#pragma once
#include "cocos2d.h"
#include "GameMap.h"
#include "Player.h"

USING_NS_CC;

class CropManager
{
public:
    static CropManager* getInstance();

    // ����Ƿ���Կ���
    bool canTill(const Vec2& tilePos) const;

    // ��������
    bool tillSoil(const Vec2& tilePos);

    // ���õ�ǰ��ͼ
    void setGameMap(GameMap* map) { _gameMap = map; }

    // ����������¼�
    void onMouseDown(const Vec2& mousePos, Player* player);

private:
    CropManager() {}
    static CropManager* _instance;
    GameMap* _gameMap = nullptr;

    /*
    �����ǹ���ͼ�鼯ID��˵����
    1.����TiledĬ��0�ǿ�ͼ�飬���ز�ͼ���1��ʼ����˿�����ID��Ҫ��1����ʵ��ID;
    2.����ʹ�õ�ID�Ѿ���һ
    */

    // �ɿ��ѵ�ͼ��ID�б�
    const std::vector<int> TILLABLE_TILES = { 207, 227, 228, 232, 464 };

    // ���Ѻ��ͼ��ID
    const int TILLED_TILE_ID = 681;
};