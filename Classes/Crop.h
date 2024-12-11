#pragma once
#include "cocos2d.h"

USING_NS_CC;

class Crop : public cocos2d::Sprite {
public:
    Crop(const std::string& cropName, int growTime, int waterNeeded, const std::string& spriteName);
    virtual ~Crop();

    // ���úͻ�ȡũ�������ֲ״̬
    virtual void plant(int startDay);
    virtual void water();
    virtual bool isMatured() const;
    virtual bool canHarvest() const;

    // ����ũ���������״̬
    virtual void update(int currentDay);

    // ��ȡũ���������
    const std::string& getCropName() const;

    // �ջ�ũ����
    virtual void harvest();

protected:
    std::string _cropName;    // ũ��������
    int _growTime;            // ������������
    int _waterNeeded;         // ÿ�������ˮ��
    int _currentGrowthTime;   // ��ǰ����ʱ��
    bool _isWatered;          // �Ƿ�ˮ
    bool _isMatured;          // �Ƿ����
    bool _isHarvested;        // �Ƿ����ջ�
    int _plantDay;            // ��ֲ������
};
