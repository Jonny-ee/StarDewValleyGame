#ifndef __GAMEMAP_H__
#define __GAMEMAP_H__

#include "cocos2d.h"
#include <string>

// 定义传送点信息的结构体
struct TransitionInfo {
    std::string targetMap;       // 目标地图名称
    cocos2d::Vec2 targetTilePos; // 目标地图中的位置（瓦片坐标）
    float sourceX;
    float sourceY;
};

class GameMap : public cocos2d::Node {
public:
    static GameMap* create(const std::string& mapName);
    bool init(const std::string& mapName);
    std::string getMapName() const;

    // 坐标转换方法
    cocos2d::Vec2 convertToTileCoord(const cocos2d::Vec2& position) const;
    cocos2d::Vec2 convertToWorldCoord(const cocos2d::Vec2& tileCoord) const;

    // 检查传送点
    bool checkForTransition(const cocos2d::Vec2& tilePos, TransitionInfo& outTransition) const;

    // 获取TMX地图对象
    cocos2d::TMXTiledMap* getTileMap() const { return _tileMap; }

    // 检查指定位置是否可通行
    bool isWalkable(const cocos2d::Vec2& worldPos) const;

private:
    std::string _mapName;
    cocos2d::TMXTiledMap* _tileMap=nullptr;

    // 从对象属性中解析传送点信息
    bool parseTransitionProperties(const cocos2d::ValueMap& properties, TransitionInfo& outTransition) const;
};

#endif // __GAMEMAP_H__