#pragma once
#include "Crop.h"

class Tomato : public Crop
{
public:
    static Tomato* create(const Vec2& position);
    virtual bool init() override;
    // 生长阶段更新
    void updateGrowthStage(int stage);      // 成长阶段显示更新
    void updateWaterStatus(int waterLevel); // 水分状态显示更新

    // 获取作物特性
    static int getTotalGrowthStages() { return TOTAL_GROWTH_STAGES; }
    static int getDaysPerStage() { return DAYS_PER_STAGE; }
    static bool canGrowToday(bool isWatered) { return isWatered; }

private:
    Tomato();
    Sprite* _waterStatusSprite;                 // 水分状态图标
    static const int TOTAL_GROWTH_STAGES = 4;   // 总生长阶段数（0-3）
    static const int DAYS_PER_STAGE = 2;        // 每个阶段的天数
};