#pragma once
#include "Crop.h"

class Corn : public Crop
{
public:
    static Corn* create(const Vec2& position);
    virtual bool init() override;

    // 生长阶段更新
    void updateGrowthStage(int stage);      // 成长阶段显示更新
    void updateWaterStatus(int waterLevel); // 水分状态显示更新
private:
    Corn();
    Sprite* _cropSprite;
    Sprite* _waterStatusSprite;         // 水分状态图标
    const int TOTAL_GROWTH_STAGES = 5;  // 总生长阶段数
    const int DAYS_PER_STAGE = 2;       // 每个阶段的天数
};