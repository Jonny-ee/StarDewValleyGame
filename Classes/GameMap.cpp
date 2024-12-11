#include "GameMap.h"

USING_NS_CC;

GameMap* GameMap::create(const std::string& mapName) {
    GameMap* ret = new (std::nothrow) GameMap();
    if (ret && ret->init(mapName)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameMap::init(const std::string& mapName) {
    if (!Node::init()) {
        //CCLOG("GameMap: Node init failed");
        return false;
    }

    _mapName = mapName;
    
   
    std::string mapPath = "maps/" + mapName + ".tmx";

    // 输出完整路径用于调试
    auto fileUtils = FileUtils::getInstance();
    std::string fullMapPath = fileUtils->fullPathForFilename(mapPath);
    //CCLOG("Attempting to load map from: %s", fullMapPath.c_str());
    //CCLOG("File exists: %s", fileUtils->isFileExist(fullMapPath) ? "yes" : "no");



    _tileMap = TMXTiledMap::create(mapPath);
    if (!_tileMap) {
        //CCLOG("GameMap: Failed to create TMX map from path: %s", mapPath.c_str());
        return false;
    }

    // 检查图块集
    ValueMap& properties = _tileMap->getProperties();
    if (properties.find("tilesets") != properties.end()) {
        ValueVector tilesets = properties["tilesets"].asValueVector();
        //CCLOG("Number of tilesets: %d", (int)tilesets.size());
        for (const auto& tileset : tilesets) {
            ValueMap tilesetInfo = tileset.asValueMap();
            //CCLOG("Tileset info:");
            //CCLOG("- Source: %s", tilesetInfo["source"].asString().c_str());
            
            // 检查图块集图片是否存在
            std::string tsPath = tilesetInfo["source"].asString();
            std::string fullTsPath = fileUtils->fullPathForFilename(tsPath);
            //CCLOG("- Full path: %s", fullTsPath.c_str());
            //CCLOG("- Exists: %s", fileUtils->isFileExist(fullTsPath) ? "yes" : "no");
        }
    } else {
        //CCLOG("No tilesets found in map properties");
    }

    // 获取地图和屏幕尺寸
    Size mapSize = _tileMap->getMapSize();
    Size tileSize = _tileMap->getTileSize();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    // 输出所有图层信息
    //CCLOG("Checking all layers in TMX:");
    auto allLayers = _tileMap->getChildren();
    int layerCount = 0;
    for (const auto& child : allLayers) {
        auto layer = dynamic_cast<TMXLayer*>(child);
        if (layer) {
            layerCount++;
            //CCLOG("Found layer: %s", layer->getLayerName().c_str());
            // 确保图层可见
            layer->setVisible(true);
            
            // 输出图层属性
            auto layerProperties = layer->getProperties();
            //CCLOG("Layer properties:");
            for (const auto& prop : layerProperties) {
                //CCLOG("- %s: %s", prop.first.c_str(), prop.second.asString().c_str());
            }
        }
    }
    //CCLOG("Total layers found: %d", layerCount);

    //地图缩放比例
    float scale = 2.5f;
    
    //CCLOG("Map original size: %.0f x %.0f", mapSize.width * tileSize.width, mapSize.height * tileSize.height);
    //CCLOG("Screen size: %.0f x %.0f", visibleSize.width, visibleSize.height);
    //CCLOG("Calculated scale: %.2f", scale);

    // 设置地图缩放和位置
    _tileMap->setScale(scale);
    this->addChild(_tileMap);

    return true;
}

std::string GameMap::getMapName() const {
    return _mapName;
}

Vec2 GameMap::convertToWorldCoord(const Vec2& tileCoord) const {
    if (!_tileMap) {
        //CCLOG("GameMap: Warning - Trying to convert coordinates with null tilemap");
        return Vec2::ZERO;
    }

    Size tileSize = _tileMap->getTileSize();
    Size mapSize = _tileMap->getMapSize();
    float scale = _tileMap->getScale();

    // X坐标：方向一致，直接乘以瓦片宽度和缩放
    float x = tileCoord.x * tileSize.width*scale;
    // Y坐标：从地图底部开始计算
   // Tiled的Y是从上往下，而我们需要从下往上
    float y = (mapSize.height- tileCoord.y) * tileSize.height * scale;

    //CCLOG("坐标转换：瓦片坐标(%.0f, %.0f) -> 世界坐标(%.1f, %.1f)",tileCoord.x, tileCoord.y, x, y);

    return Vec2(x, y);
}

Vec2 GameMap::convertToTileCoord(const Vec2& worldPosition) const {
    if (!_tileMap) {
        //CCLOG("GameMap: Warning - Trying to convert coordinates with null tilemap");
        return Vec2::ZERO;
    }

    Size tileSize = _tileMap->getTileSize();//瓦片大小，本程序都是16*16
    Size mapSize = _tileMap->getMapSize();//地图大小
    float scale = _tileMap->getScale();

    // X坐标：直接除以瓦片宽度和缩放
    int x = worldPosition.x / (tileSize.width* scale);
    // Y坐标：从地图底部转换回Tiled的从上往下
    int y = mapSize.height - worldPosition.y / (scale * tileSize.height);

    //CCLOG("坐标转换：世界坐标(%.1f, %.1f) -> 瓦片坐标(%d, %d)",worldPosition.x, worldPosition.y, x, y);

    return Vec2(x, y);
}

bool GameMap::checkForTransition(const Vec2& tilePos, TransitionInfo& outTransition) const {
    // 首先检查坐标是否在地图范围内
    Size mapSize = _tileMap->getMapSize();
    if (tilePos.x < 0 || tilePos.x >= mapSize.width ||
        tilePos.y < 0 || tilePos.y >= mapSize.height) {
        //CCLOG("传送检测：坐标 (%.1f, %.1f) 超出地图范围", tilePos.x, tilePos.y);
        return false;
    }

    ValueMap& properties = _tileMap->getProperties();
    if (properties.find("Warp") != properties.end()) {
        std::string warpStr = properties.at("Warp").asString();
        std::istringstream iss(warpStr);
        float sourceX, sourceY;
        std::string targetMap;
        float targetX, targetY;

        while (iss >> sourceX >> sourceY >> targetMap >> targetX >> targetY) {
            // 使用更小的容错范围，并确保在地图边界内
            if (std::abs(tilePos.x - sourceX) < 0.3f &&
                std::abs(tilePos.y - sourceY) < 0.3f) {

                //CCLOG("触发传送点: (%.1f, %.1f) -> %s (%.1f, %.1f)",sourceX, sourceY, targetMap.c_str(), targetX, targetY);

                // 保存源坐标和目标信息
                outTransition.sourceX = sourceX;
                outTransition.sourceY = sourceY;
                outTransition.targetMap = targetMap;
                outTransition.targetTilePos = Vec2(targetX, targetY);
                return true;
            }
        }
    }
    return false;
}

bool GameMap::parseTransitionProperties(const ValueMap& properties, TransitionInfo& outTransition) const {
    // 检查必要的属性是否存在（意属性名要和Tiled中设置的完全一致）
    if (properties.find("TargetMap") == properties.end() ||
        properties.find("TargetX") == properties.end() ||
        properties.find("TargetY") == properties.end()) {
        return false;
    }

    // 解析传送点信息
    outTransition.targetMap = properties.at("TargetMap").asString();
    outTransition.targetTilePos.x = properties.at("TargetX").asFloat();
    outTransition.targetTilePos.y = properties.at("TargetY").asFloat();

    return true;
}

//参数为世界坐标
//本函数会将世界坐标改为地图坐标tilePos，判断当前地图坐标上是否有障碍物
bool GameMap::isWalkable(const Vec2& worldPos) const {
    if (!_tileMap) {
        //CCLOG("碰撞检测：地图未加载");
        return false;
    }

    Vec2 tilePos = convertToTileCoord(worldPos);

    // 获取 Buildings 层
    TMXLayer* buildings1Layer = _tileMap->getLayer("Buildings1");
    if (buildings1Layer) {
        // 检查该位置是否有图块
        int tileGID = buildings1Layer->getTileGIDAt(tilePos);
        if (tileGID > 0) {  // 如果GID大于0，说明有图块
            //CCLOG("碰撞检测：位置 (%.0f, %.0f) 在Buildings层上有图块，不可通行", tilePos.x, tilePos.y);
            return false;  // 有图块就不能通过
        }
    }
    // 获取 Buildings 层
    TMXLayer* buildings2Layer = _tileMap->getLayer("Buildings2");
    if (buildings2Layer) {
        // 检查该位置是否有图块
        int tileGID = buildings2Layer->getTileGIDAt(tilePos);
        if (tileGID > 0) {  // 如果GID大于0，说明有图块
            //CCLOG("碰撞检测：位置 (%.0f, %.0f) 在Buildings层上有图块，不可通行", tilePos.x, tilePos.y);
            return false;  // 有图块就不能通过
        }
    }
  
    // 检查坐标是否在地图范围内
    Size mapSize = _tileMap->getMapSize();
    if (tilePos.x < 0 || tilePos.x >= mapSize.width ||
        tilePos.y < 0 || tilePos.y >= mapSize.height) {
        //CCLOG("碰撞检测：位置 (%.0f, %.0f) 超出地图范围", tilePos.x, tilePos.y);
        return false;
    }

    //CCLOG("碰撞检测：位置 (%.0f, %.0f) 可以通行", tilePos.x, tilePos.y);
    return true;  // 没有图块或没有Buildings层，则可以通行
}

