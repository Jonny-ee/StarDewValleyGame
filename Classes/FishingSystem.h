#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"

class FishingSystem
{
public:
    static FishingSystem* getInstance();

    // 检查是否可以钓鱼
    bool canFish(const cocos2d::Vec2& playerPos, Player* player);

    // 开始钓鱼
    void startFishing();

    // 结束钓鱼并获得鱼
    void finishFishing();

    // 初始化钓鱼区域
    void initFishingAreas(GameMap* gameMap);

    bool isCurrentlyFishing() const { return isFishing; }
private:
    FishingSystem() = default;
    static FishingSystem* _instance;

    // 检查位置是否在钓鱼区域内
    bool isInFishingArea(const cocos2d::Vec2& position);

    // 检查是否装备了鱼竿
    bool hasEquippedFishingRod(Player* player);

    std::vector<cocos2d::Rect> fishingAreas; // 存储钓鱼区域
    bool isFishing = false; // 是否正在钓鱼

    std::chrono::steady_clock::time_point fishingStartTime;
    const int FISHING_DURATION = 8; // 钓鱼需要的时间（秒）
};