#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"

class FishingSystem
{
public:
    static FishingSystem* getInstance();

    // ����Ƿ���Ե���
    bool canFish(const cocos2d::Vec2& playerPos, Player* player);

    // ��ʼ����
    void startFishing();

    // �������㲢�����
    void finishFishing();

    // ��ʼ����������
    void initFishingAreas(GameMap* gameMap);

    bool isCurrentlyFishing() const { return isFishing; }
private:
    FishingSystem() = default;
    static FishingSystem* _instance;

    // ���λ���Ƿ��ڵ���������
    bool isInFishingArea(const cocos2d::Vec2& position);

    // ����Ƿ�װ�������
    bool hasEquippedFishingRod(Player* player);

    std::vector<cocos2d::Rect> fishingAreas; // �洢��������
    bool isFishing = false; // �Ƿ����ڵ���

    std::chrono::steady_clock::time_point fishingStartTime;
    const int FISHING_DURATION = 8; // ������Ҫ��ʱ�䣨�룩
};