#include "LightManager.h"
#include "GameTime.h"

USING_NS_CC;

LightManager* LightManager::_instance = nullptr;

LightManager* LightManager::getInstance() {
    if (_instance == nullptr) {
        _instance = new LightManager();
    }
    return _instance;
}

void LightManager::initWithMap(GameMap* gameMap) {
    cleanup();

    _gameMap = gameMap;

    // 获取路灯发光层
    _lampLightsLayer = _gameMap->getTileMap()->getLayer("Front-LightLamp");
    if (_lampLightsLayer) {
        _lampLightsLayer->setVisible(false);
    }

    createNightFilter();
    createLightEffects();
}

void LightManager::cleanup() {
    if (_nightFilter) {
        _nightFilter->removeFromParent();
        _nightFilter = nullptr;
    }

    for (auto light : _lightSources) {
        light->removeFromParent();
    }
    _lightSources.clear();

    _gameMap = nullptr;
    _lampLightsLayer = nullptr;
}

void LightManager::createNightFilter() {
    if (!_gameMap) return;

    auto tileMap = _gameMap->getTileMap();
    Size mapSize = tileMap->getMapSize();
    Size tileSize = tileMap->getTileSize();
    float scale = tileMap->getScale();
    Size mapPixelSize = Size(mapSize.width * tileSize.width * scale,
        mapSize.height * tileSize.height * scale);

    _nightFilter = LayerColor::create(Color4B(0, 0, 20, 0),
        mapPixelSize.width,
        mapPixelSize.height);
    _nightFilter->setPosition(Vec2::ZERO);
    _gameMap->addChild(_nightFilter, 997);
}

void LightManager::createLightEffects() {
    if (!_gameMap) return;

    auto tileMap = _gameMap->getTileMap();
    _lampLightsLayer = tileMap->getLayer("Front-LightLamp");
    if (!_lampLightsLayer) return;

    // 获取地图尺寸
    Size mapSize = tileMap->getMapSize();

    // 遍历Front-LightLamp层的所有瓦片
    for (int x = 0; x < mapSize.width; x++) {
        for (int y = 0; y < mapSize.height; y++) {
            // 检查该位置是否有灯光瓦片
            int tileGID = _lampLightsLayer->getTileGIDAt(Vec2(x, y));
            if (tileGID > 0) {  // 如果有瓦片
                // 使用GameMap的转换函数获取世界坐标
                Vec2 worldPos = _gameMap->convertToWorldCoord(Vec2(x, y));

                // 创建光源
                auto lightSprite = Sprite::create("Light.png");
                if (lightSprite) {
                    // 设置统一的光源大小
                    float scale = 6.0f;  // 可以调整这个值来改变光源大小
                    lightSprite->setScale(scale);
                    // 设置更强的光照效果
                    lightSprite->setOpacity(255);  // 设置最大不透明度
                    // 设置混合模式
                    BlendFunc blend = { GL_SRC_ALPHA, GL_ONE };
                    lightSprite->setBlendFunc(blend);

                    // 设置位置
                    lightSprite->setPosition(worldPos);
                    lightSprite->setVisible(false);

                    // 添加到地图
                    _gameMap->addChild(lightSprite, 998);
                    _lightSources.push_back(lightSprite);
                }
            }
        }
    }

    // 初始时设置发光层不可见
    _lampLightsLayer->setVisible(false);
}

GLubyte LightManager::calculateNightOpacity(int currentHour) {
    if (currentHour >= 0 && currentHour < 5) {
        return 180;
    }
    else if (currentHour >= 5 && currentHour < 7) {
        float progress = (currentHour - 5) / 2.0f;
        return static_cast<GLubyte>(180 * (1 - progress));
    }
    else if (currentHour >= 7 && currentHour < 18) {
        return 0;
    }
    else if (currentHour >= 18 && currentHour < 20) {
        float progress = (currentHour - 18) / 2.0f;
        return static_cast<GLubyte>(180 * progress);
    }
    else {
        return 180;
    }
}

void LightManager::update() {
    GameTime* gameTime = GameTime::getInstance();
    int currentHour = gameTime->getHour();

    GLubyte opacity = calculateNightOpacity(currentHour);

    if (_nightFilter) {
        _nightFilter->setOpacity(opacity);
    }

    bool isNight = (opacity > 0);
    updateLightEffects(isNight, opacity);
}

void LightManager::updateLightEffects(bool isNight, GLubyte opacity) {
    if (_lampLightsLayer) {
        _lampLightsLayer->setVisible(isNight);
        _lampLightsLayer->setOpacity(255);
    }

    for (auto light : _lightSources) {
        light->setVisible(isNight);
        light->setOpacity(170);
    }
}