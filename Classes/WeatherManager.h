#pragma once
#ifndef WEATHER_MANAGER_H
#define WEATHER_MANAGER_H

#include "Weather.h"
#include "Rain.h"
#include "Drought.h"
#include "NormalWeather.h"
class WeatherManager {
public:
    static WeatherManager* getInstance();  // 获取单例实例的方法

    void setWeather(Weather* weather);  // 设置当前天气，会停用之前的天气并激活新的天气
    Weather* getCurrentWeather();  // 获取当前激活的天气
    void randomRefreshWeather();  // 用于随机刷新天气
private:
    WeatherManager();
    ~WeatherManager();

    static WeatherManager* _instance;  // 单例实例指针
    Weather* _currentWeather;  // 当前激活的天气

    // 删除拷贝构造和赋值操作符
    WeatherManager(const WeatherManager&) = delete;
    WeatherManager& operator=(const WeatherManager&) = delete;
};
#endif //WEATHER_MANAGER_H
