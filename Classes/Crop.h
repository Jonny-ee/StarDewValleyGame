#pragma once
#include "cocos2d.h"

USING_NS_CC;

class Crop : public cocos2d::Sprite {
public:
    Crop(const std::string& cropName, int growTime, int waterNeeded, const std::string& spriteName);
    virtual ~Crop();

    // 设置和获取农作物的种植状态
    virtual void plant(int startDay);
    virtual void water();
    virtual bool isMatured() const;
    virtual bool canHarvest() const;

    // 更新农作物的生长状态
    virtual void update(int currentDay);

    // 获取农作物的名字
    const std::string& getCropName() const;

    // 收获农作物
    virtual void harvest();

protected:
    std::string _cropName;    // 农作物名字
    int _growTime;            // 生长所需天数
    int _waterNeeded;         // 每天所需的水量
    int _currentGrowthTime;   // 当前生长时间
    bool _isWatered;          // 是否浇水
    bool _isMatured;          // 是否成熟
    bool _isHarvested;        // 是否已收获
    int _plantDay;            // 种植的天数
};
