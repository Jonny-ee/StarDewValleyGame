#pragma once
#ifndef __LIGHT_MANAGER_H__
#define __LIGHT_MANAGER_H__

#include "cocos2d.h"
#include "GameMap.h"
#include <vector>

class LightManager {
public:
    static LightManager* getInstance();

    // 删除拷贝构造和赋值操作符
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;

    // 初始化光照系统
    void initWithMap(GameMap* gameMap);

    // 更新光照效果
    void update();

    // 清理当前地图的光照效果
    void cleanup();

private:
    LightManager() = default;
    static LightManager* _instance;

    GameMap* _gameMap = nullptr;
    cocos2d::LayerColor* _nightFilter = nullptr;
    cocos2d::TMXLayer* _lampLightsLayer = nullptr;
    std::vector<cocos2d::Sprite*> _lightSources;

    void createNightFilter();
    void createLightEffects();
    GLubyte calculateNightOpacity(int currentHour);
    void updateLightEffects(bool isNight, GLubyte opacity);
};

#endif // __LIGHT_MANAGER_H__