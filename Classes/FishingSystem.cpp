#include "FishingSystem.h"
#include "ItemSystem.h"

USING_NS_CC;

FishingSystem* FishingSystem::_instance = nullptr;

FishingSystem* FishingSystem::getInstance() {
    if (!_instance) {
        _instance = new FishingSystem();
    }
    return _instance;
}

void FishingSystem::initFishingAreas(GameMap* gameMap) {
    fishingAreas.clear();

    // 打印当前地图名称以便调试
    CCLOG("Current map name: %s", gameMap->getMapName().c_str());

    // 检查当前地图是否是 Farm
    if (gameMap->getMapName() == "Farm") {  // 移除 .tmx 后缀
        // 只在农场地图中添加钓鱼区域
        fishingAreas.push_back(cocos2d::Rect(820, 510, 240, 280));
        CCLOG("Added fishing area in Farm map: x=820, y=510");
    }
    else {
        CCLOG("Current map is not Farm, no fishing areas added");
    }
}

bool FishingSystem::isInFishingArea(const Vec2& position) {
    // 如果没有钓鱼区域（不是农场地图），直接返回 false
    if (fishingAreas.empty()) {
        return false;
    }

    for (const auto& area : fishingAreas)
    {
        CCLOG("Checking position (%.1f, %.1f) against fishing area", position.x, position.y);
        if (area.containsPoint(position))
        {
            CCLOG("Player is in fishing area!");
            return true;
        }
    }
    return false;
}

bool FishingSystem::hasEquippedFishingRod(Player* player)
{
    return player->getCurrentTool() == Player::ToolType::ROD;
}

bool FishingSystem::canFish(const Vec2& playerPos, Player* player)
{
    // 检查是否已经在钓鱼
    if (isFishing) {
        CCLOG("Already fishing...");
        return false;
    }

    // 检查是否装备鱼竿
    if (!hasEquippedFishingRod(player))
    {
        CCLOG("Need to equip fishing rod first");
        return false;
    }

    // 检查是否在钓鱼区域
    if (!isInFishingArea(playerPos))
    {
        CCLOG("Need to be near water to fish");
        return false;
    }

    CCLOG("Ready to start fishing!");
    return true;
}

void FishingSystem::startFishing()
{
    if (!isFishing)
    {
        isFishing = true;
        fishingStartTime = std::chrono::steady_clock::now();
        CCLOG("Start fishing...");
    }
}

void FishingSystem::finishFishing()
{
    if (isFishing)
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - fishingStartTime).count();

        if (elapsedTime >= FISHING_DURATION)
        {
            // 生成0-100的随机数
            int randomNum = rand() % 100;

            // 70%的概率成功
            if (randomNum < 70)
            {
                CCLOG("Fishing success! Time used: %ds", (int)elapsedTime);
                int randomNum = rand() % 100;
                if (randomNum < 80)
                    ItemSystem::getInstance()->addItem("fish", 1);
                else    // 综合概率14%左右
                    ItemSystem::getInstance()->addItem("mermaid's KISS(*)", 1);
            }
            else
            {
                CCLOG("Fish escaped! Better luck next time!");
            }

            isFishing = false;
        }
        else
        {
            CCLOG("Not enough time, need to wait %ds more", FISHING_DURATION - (int)elapsedTime);
            // 时间不够时也结束当前钓鱼，让玩家重新开始
            isFishing = false;
        }
    }
}