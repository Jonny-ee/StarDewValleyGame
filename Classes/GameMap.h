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
// 来存储地图对象的状态的机构体
struct MapObject {
    std::string type;
    // 其他需要的属性...
};
class GameMap : public cocos2d::Node {
public:
    // 获取单例实例
    static GameMap* getInstance();

    // 初始化或切换地图
    bool loadMap(const std::string& mapName);

    // 删除拷贝构造和赋值操作符
    GameMap(const GameMap&) = delete;
    GameMap& operator=(const GameMap&) = delete;

    static GameMap* create(const std::string& mapName);
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

    GameMap(); // 私有构造函数
    static GameMap* _instance;

    // 存储所有地图的状态
    std::map<std::string, std::vector<MapObject>> _mapStates;
    void saveCurrentMapState();
    void loadMapState();
    std::string _mapName;
    cocos2d::TMXTiledMap* _tileMap=nullptr;

    // 从对象属性中解析传送点信息
    bool parseTransitionProperties(const cocos2d::ValueMap& properties, TransitionInfo& outTransition) const;
};

#endif // __GAMEMAP_H__