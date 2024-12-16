#include "ResourceManager.h"
#include "GameMap.h"  // 确保引用 GameMap 的头文件
#include "cocos2d.h"

ResourceManager* ResourceManager::getInstance()
{
    static ResourceManager instance;
    return &instance;
}

bool ResourceManager::init(GameMap* gameMap)
{
    if (!gameMap)
    {
        cocos2d::log("初始化 ResourceManager 失败：gameMap 为 nullptr");
        return false;
    }

    _gameMap = gameMap;

    loadResources();

    return true;
}

void ResourceManager::loadResources()
{
    // 加载 Path.png 图块集
    loadPathTileset();

    // 添加其他资源加载，如果有的话
}

void ResourceManager::loadPathTileset()
{
    // 假设 Path.plist 位于 TileSheets 文件夹
    std::string plistPath = "TileSheets/Path.plist";
    std::string pngPath = "TileSheets/Path.png";

    // 检查图块集是否已加载，避免重复加载
    if (!cocos2d::SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(plistPath))
    {
        cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath, pngPath);
    }

    // 定义图块名称与ID的映射
    // 请确保 Path.plist 中存在这些名称的帧
    tileIDMap["grass"] = 14;
    tileIDMap["stone"] = 17;
    tileIDMap["replacement"] = REPLACEMENT_TILE_ID; // 227

    cocos2d::log("Path.png 图块集已加载并映射");
}

int ResourceManager::getTileID(const std::string& tileName) const
{
    auto it = tileIDMap.find(tileName);
    if (it != tileIDMap.end())
    {
        return it->second;
    }
    else
    {
        cocos2d::log("未找到图块名称：%s 的ID", tileName.c_str());
        return -1; // 或者抛出异常，根据需求处理
    }
}