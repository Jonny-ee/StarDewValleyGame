#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "cocos2d.h"
#include "GameMap.h"

class MapManager : public cocos2d::Node {
public:
    // 创建 MapManager 实例
    static MapManager* create();

    // 初始化 MapManager
    virtual bool init();

    // 加载指定名称的地图，并切换至该地图
    void loadMap(const std::string& mapName, const cocos2d::Vec2& playerStartPos);

    // 获取当前活动地图
    GameMap* getCurrentMap() const;

    // 获取当前地图的名称
    std::string getCurrentMapName() const;

private:
    // 记录当前地图实例
    GameMap* _currentMap;
    // 当前地图名称
    std::string _currentMapName;
    // 玩家在当前地图中的位置
    cocos2d::Vec2 _playerPosition;
};

#endif // MAPMANAGER_H
