#pragma once
#include "Crop.h"

class Corn : public Crop
{
public:
    static Corn* create(const Vec2& position);
    virtual bool init() override;

    // 生长阶段更新
    void updateGrowthStage(int stage);

private:
    Corn();

    const int TOTAL_GROWTH_STAGES = 5;  // 总生长阶段数
    const int DAYS_PER_STAGE = 2;       // 每个阶段的天数
};