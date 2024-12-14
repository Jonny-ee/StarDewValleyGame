#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"

/*
 * 钓鱼系统类
 * 功能：管理游戏中的钓鱼相关功能
 * 1.钓鱼区域管理
 * 2.钓鱼状态控制
 * 3.钓鱼结果处理
 * 4.提示信息显示
 */
class FishingSystem
{
public:
    static FishingSystem* getInstance();                            // 获取钓鱼系统单例实例
    bool canFish(const cocos2d::Vec2& playerPos, Player* player);   // 检查是否可以钓鱼
    void startFishing();                                            // 开始钓鱼
    void finishFishing();                                           // 结束钓鱼并获得鱼
    void initFishingAreas(GameMap* gameMap);                        // 初始化钓鱼区域
    bool isCurrentlyFishing() const { return isFishing; }           // 获取当前是否在钓鱼
    cocos2d::Label* getTipLabel() const { return tipLabel; }        // 获取提示标签

private:
    FishingSystem() = default;
    static FishingSystem* _instance;

    bool isInFishingArea(const cocos2d::Vec2& position);            // 检查位置是否在钓鱼区域内
    bool hasEquippedFishingRod(Player* player);                     // 检查是否装备了鱼竿

    std::vector<cocos2d::Rect> fishingAreas;                        // 存储钓鱼区域
    bool isFishing = false;                                         // 是否正在钓鱼

    std::chrono::steady_clock::time_point fishingStartTime;         // 钓鱼开始时间
    const int FISHING_DURATION = 6;                                 // 钓鱼需要的时间（秒）

    cocos2d::Label* tipLabel = nullptr;                             // 提示文本标签
    void showTip(const std::string& text, float duration = 0.0f);   // 显示提示文本
    void hideTip();                                                 // 隐藏提示文本
    bool showingResult = false;                                     // 控制是否正在显示结果

    /*
     * 重置钓鱼状态
     * 功能：重置所有钓鱼相关状态和提示
     */
    void resetFishingState()
    {
        isFishing = false;
        showingResult = false;
        // 重置开始时间
        fishingStartTime = std::chrono::steady_clock::now();
        // 停止所有提示动作
        if (tipLabel)
        {
            tipLabel->stopAllActions();
        }
        hideTip();
    }
};