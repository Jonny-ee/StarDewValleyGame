#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"

/*
 * ����ϵͳ��
 * ���ܣ�������Ϸ�еĵ�����ع���
 * 1.�����������
 * 2.����״̬����
 * 3.����������
 * 4.��ʾ��Ϣ��ʾ
 */
class FishingSystem
{
public:
    static FishingSystem* getInstance();                            // ��ȡ����ϵͳ����ʵ��
    bool canFish(const cocos2d::Vec2& playerPos, Player* player);   // ����Ƿ���Ե���
    void startFishing();                                            // ��ʼ����
    void finishFishing();                                           // �������㲢�����
    void initFishingAreas(GameMap* gameMap);                        // ��ʼ����������
    bool isCurrentlyFishing() const { return isFishing; }           // ��ȡ��ǰ�Ƿ��ڵ���
    cocos2d::Label* getTipLabel() const { return tipLabel; }        // ��ȡ��ʾ��ǩ

private:
    FishingSystem() = default;
    static FishingSystem* _instance;

    bool isInFishingArea(const cocos2d::Vec2& position);            // ���λ���Ƿ��ڵ���������
    bool hasEquippedFishingRod(Player* player);                     // ����Ƿ�װ�������

    std::vector<cocos2d::Rect> fishingAreas;                        // �洢��������
    bool isFishing = false;                                         // �Ƿ����ڵ���

    std::chrono::steady_clock::time_point fishingStartTime;         // ���㿪ʼʱ��
    const int FISHING_DURATION = 6;                                 // ������Ҫ��ʱ�䣨�룩

    cocos2d::Label* tipLabel = nullptr;                             // ��ʾ�ı���ǩ
    void showTip(const std::string& text, float duration = 0.0f);   // ��ʾ��ʾ�ı�
    void hideTip();                                                 // ������ʾ�ı�
    bool showingResult = false;                                     // �����Ƿ�������ʾ���

    /*
     * ���õ���״̬
     * ���ܣ��������е������״̬����ʾ
     */
    void resetFishingState()
    {
        isFishing = false;
        showingResult = false;
        // ���ÿ�ʼʱ��
        fishingStartTime = std::chrono::steady_clock::now();
        // ֹͣ������ʾ����
        if (tipLabel)
        {
            tipLabel->stopAllActions();
        }
        hideTip();
    }
};