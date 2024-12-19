#pragma once
#ifndef NORMAL_WEATHER_H
#define NORMAL_WEATHER_H
#include "cocos2d.h"
#include "Weather.h"

class NormalWeather : public Weather {
public:
    static NormalWeather* create();

    virtual void update(float dt) override;
    virtual std::string getWeatherName() const override { return "Normal"; }

private:
    NormalWeather() {};
    bool init();
};
#endif//NORMAL_WEATHER_H