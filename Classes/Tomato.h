#pragma once
#include "Crop.h"

class Tomato : public Crop
{
public:
    static Tomato* create(const Vec2& position);
    virtual bool init() override;
    // �����׶θ���
    void updateGrowthStage(int stage);      // �ɳ��׶���ʾ����
    void updateWaterStatus(int waterLevel); // ˮ��״̬��ʾ����

    // ��ȡ��������
    static int getTotalGrowthStages() { return TOTAL_GROWTH_STAGES; }
    static int getDaysPerStage() { return DAYS_PER_STAGE; }
    static bool canGrowToday(bool isWatered) { return isWatered; }

private:
    Tomato();
    Sprite* _waterStatusSprite;                 // ˮ��״̬ͼ��
    static const int TOTAL_GROWTH_STAGES = 4;   // �������׶�����0-3��
    static const int DAYS_PER_STAGE = 2;        // ÿ���׶ε�����
};