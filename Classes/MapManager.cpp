#include "MapManager.h"
#include "GameMap.h"  // 引入 GameMap 头文件
#include "cocos2d.h"  // 引入 cocos2d 头文件

USING_NS_CC;

// 创建 MapManager 实例
MapManager* MapManager::create() {
    MapManager* ret = new MapManager();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

// 初始化 MapManager
bool MapManager::init() {
    if (!Node::init()) {
        return false;
    }

    // 初始时当前地图为空
    _currentMap = nullptr;
    _currentMapName = "";
    _playerPosition = Vec2(0, 0);

    return true;
}

// 加载指定名称的地图，并切换至该地图
void MapManager::loadMap(const std::string& mapName, const cocos2d::Vec2& playerStartPos) {
    CCLOG("MapManager: Loading map: %s", mapName.c_str());

    if (_currentMap != nullptr) {
        _currentMap->removeAllChildren();  // 移除所有子节点
        _currentMap->removeFromParent();// 从父节点中移除
        _currentMap = nullptr;
    }

    _currentMap = GameMap::create(mapName);
    if (_currentMap == nullptr) {
        CCLOG("MapManager: Failed to create map!");
        return;
    }

    // 先添加地图到场景
    this->addChild(_currentMap);
   // _currentMap->setPosition(Vec2(0,0));

    _currentMapName = mapName;
    _playerPosition = playerStartPos;
    
    CCLOG("MapManager: Map loaded and positioned successfully");
}

// 获取当前活动地图
GameMap* MapManager::getCurrentMap() const {
    return _currentMap;
}

// 获取当前地图的名称
std::string MapManager::getCurrentMapName() const {
    return _currentMapName;
}

