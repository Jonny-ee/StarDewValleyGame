#pragma once
#include "cocos2d.h"
#include <unordered_map>
#include <string>

// 前向声明 GameMap 类
class GameMap;

class ResourceManager
{
public:
    // 获取单例实例
    static ResourceManager* getInstance();

    // 初始化资源管理器，接受 GameMap 指针
    bool init(GameMap* gameMap);

    // 加载所有必要的资源
    void loadResources();

    // 获取特定图块的纹理ID
    int getTileID(const std::string& tileName) const;

    // 获取替换后的图块ID
    int getReplacementTileID() const { return REPLACEMENT_TILE_ID; }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    // 禁用拷贝构造和赋值
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // 图块名称与ID的映射
    std::unordered_map<std::string, int> tileIDMap;

    // 游戏地图引用
    GameMap* _gameMap = nullptr;

    // 替换图块ID
    const int REPLACEMENT_TILE_ID = 227;

    // 载入 Path.png 图块集
    void loadPathTileset();
};