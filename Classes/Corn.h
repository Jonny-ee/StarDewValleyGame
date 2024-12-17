#pragma once
#include "Crop.h"

class Corn : public Crop
{
public:
    static Corn* create(const Vec2& position);
    virtual bool init() override;

    // �����׶θ���
    void updateGrowthStage(int stage);

private:
    Corn();

    const int TOTAL_GROWTH_STAGES = 5;  // �������׶���
    const int DAYS_PER_STAGE = 2;       // ÿ���׶ε�����
};