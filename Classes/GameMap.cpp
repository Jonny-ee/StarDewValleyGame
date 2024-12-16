#include "GameMap.h"
#include"GameTime.h"
#include"LightManager.h"
USING_NS_CC;
GameMap* GameMap::_instance = nullptr;

GameMap* GameMap::getInstance() {
    if (_instance == nullptr) {
        _instance = new GameMap();
        _instance->autorelease();
    }
    return _instance;
}

GameMap::GameMap() : _tileMap(nullptr) {
    // 初始化其他必要的成员变量
}

GameMap* GameMap::create(const std::string& mapName) {
    GameMap* ret = new (std::nothrow) GameMap();
    if (ret && ret->loadMap(mapName)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameMap::loadMap(const std::string& mapName) {
    // 如果从Farm地图切换出去，保存耕地状态
    if (_tileMap && _mapName == "Farm") {
        saveCurrentMapState();
    }

    // 清理旧地图
    if (_tileMap) {
        _tileMap->removeAllChildren();
        _tileMap->release();
        _tileMap = nullptr;
    }

    _mapName = mapName;

    std::string mapPath = "maps/" + mapName + ".tmx";

    // 创建新地图
    _tileMap = TMXTiledMap::create(mapPath);
    if (!_tileMap) {
        return false;
    }
    _tileMap->retain();

    // 获取地图和屏幕尺寸
    Size mapSize = _tileMap->getMapSize();
    Size tileSize = _tileMap->getTileSize();
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 设置地图缩放
    float scale = 2.5f;
    _tileMap->setScale(scale);

    this->addChild(_tileMap);

    // 如果切换到Farm地图，加载耕地状态
    if (mapName == "Farm") {
        loadMapState();
    }
    if (_bridgeRepaired)
    {
        repairBridge();
    }
    // 初始化光照系统
    LightManager::getInstance()->initWithMap(this);

    return true;
}

void GameMap::saveCurrentMapState() {
    if (_mapName != "Farm") return;

    std::vector<MapObject> farmState;
    auto backLayer = _tileMap->getLayer("Back");
    if (!backLayer) return;

    Size mapSize = _tileMap->getMapSize();
    // 遍历地图所有瓦片
    for (int x = 0; x < mapSize.width; x++) {
        for (int y = 0; y < mapSize.height; y++) {
            int tileGID = backLayer->getTileGIDAt(Vec2(x, y));
            // 如果是耕地（TILLED_TILE_ID = 681）
            if (tileGID == 681) {
                MapObject obj;
                obj.type = "tilled";
                obj.position = Vec2(x, y);
                obj.tileGID = tileGID;
                farmState.push_back(obj);
            }
        }
    }

    _mapStates["Farm"] = farmState;
}

void GameMap::loadMapState() {
    if (_mapName != "Farm") return;

    auto it = _mapStates.find("Farm");
    if (it != _mapStates.end()) {
        auto backLayer = _tileMap->getLayer("Back");
        if (!backLayer) return;

        // 恢复所有保存的耕地
        for (const auto& obj : it->second) {
            if (obj.type == "tilled") {
                backLayer->setTileGID(obj.tileGID, obj.position);
            }
        }
    }
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

    // 检查宝箱碰撞
    if (isChestCollision(worldPos)) {
        return false;
    }

    // 检查资源碰撞
    if (isResourceCollision(worldPos)) {
        return false;
    }


    // 检查所有Buildings层
    const auto& allLayers = _tileMap->getChildren();
    for (const auto& child : allLayers) {
        auto layer = dynamic_cast<TMXLayer*>(child);
        if (layer) {
            std::string layerName = layer->getLayerName();
            if (layerName.substr(0, 9) == "Buildings") {
                // 检查该位置是否有图块
                int tileGID = layer->getTileGIDAt(tilePos);
                if (tileGID > 0) {
                    return false;  // 任何Buildings层有图块都不可通行
                }
            }
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

bool GameMap::isChestCollision(const Vec2& worldPos) const
{
    if (!_tileMap) {
        CCLOG("No tilemap loaded");
        return false;
    }

    // 转换为瓦片坐标
    Vec2 tilePos = convertToTileCoord(worldPos);

    // 获取宝箱碰撞层
    auto chestLayer = _tileMap->getLayer("CollisionChest");
    if (chestLayer) {
        // 检查该位置是否有碰撞图块
        int tileGID = chestLayer->getTileGIDAt(tilePos);
        if (tileGID > 0) {
            CCLOG("Chest collision detected at tile position (%.1f, %.1f)",
                tilePos.x, tilePos.y);
            return true;
        }
    }
    
    return false;  // 无碰撞
}

bool GameMap::isResourceCollision(const Vec2& worldPos) const {
    if (!_tileMap) {
        return false;
    }

    // 转换为瓦片坐标
    Vec2 tilePos = convertToTileCoord(worldPos);

    // 获取资源层
    auto resourceLayer = _tileMap->getLayer("resource");
    if (resourceLayer) {
        // 检查该位置是否有资源图块
        int tileGID = resourceLayer->getTileGIDAt(tilePos);
        if (tileGID > 0) {
            return true;  // 有资源，发生碰撞
        }
    }

    return false;  // 无碰撞
}

void GameMap::repairBridge() {

    // 遍历并处理所有断桥相关图层
    for (const auto& layerName : BROKEN_BRIDGE_LAYERS) {
        auto layer = _tileMap->getLayer(layerName);
        if (layer) {
                _tileMap->removeChild(layer, true);
        }
    }

    _bridgeRepaired = true;
}