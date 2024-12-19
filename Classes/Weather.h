#pragma once
#ifndef WEATHER_H
#define WEATHER_H
#include "cocos2d.h"

class Weather : public cocos2d::Ref {
public:
    virtual void update(float dt) = 0;  // 抽象方法，用于更新天气状态，不同天气有不同更新逻辑，比如下雨可能要更新雨滴位置等
    virtual std::string getWeatherName() const = 0;  // 获取天气名称的抽象方法
    bool _isActive;// 是否处于激活状态
    void setStatus() {
        _isActive = !_isActive;
    };
protected:
    Weather(): _isActive(false) {}  // 构造函数初始化激活状态为false
};
#endif //WEATHER_H