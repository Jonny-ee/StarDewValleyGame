#include "GameMap.h"
#include"GameTime.h"
#include"LightManager.h"
#include "CropManager.h"
#include<exception>
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
       if (_tileMap == nullptr)
           return false;    
     _tileMap->retain();

    // 获取地图和屏幕尺寸
    const Size mapSize = _tileMap->getMapSize();
    const Size tileSize = _tileMap->getTileSize();
    const  Size visibleSize = Director::getInstance()->getVisibleSize();

    // 设置地图缩放
    const float scale = 2.5f;
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

    initFrontLayers();

    return true;
}

void GameMap::saveCurrentMapState() {
    if (_mapName != "Farm") return;

    std::vector<MapObject> farmState;
    auto backLayer = _tileMap->getLayer("Back");
    if (!backLayer) return;

    const Size mapSize = _tileMap->getMapSize();
    // 遍历地图所有瓦片
    for (int x = 0; x < mapSize.width; x++) {
        for (int y = 0; y < mapSize.height; y++) {
            const int tileGID = backLayer->getTileGIDAt(Vec2(x, y));
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

    const Size tileSize = _tileMap->getTileSize();
    const Size mapSize = _tileMap->getMapSize();
    const  float scale = _tileMap->getScale();

    // X坐标：方向一致，直接乘以瓦片宽度和缩放
    const float x = tileCoord.x * tileSize.width*scale;
    // Y坐标：从地图底部开始计算
   // Tiled的Y是从上往下，而我们需要从下往上
    const  float y = (mapSize.height- tileCoord.y) * tileSize.height * scale;

    //CCLOG("坐标转换：瓦片坐标(%.0f, %.0f) -> 世界坐标(%.1f, %.1f)",tileCoord.x, tileCoord.y, x, y);

    return Vec2(x, y);
}

Vec2 GameMap::convertToTileCoord(const Vec2& worldPosition) const {
    if (!_tileMap) {
        //CCLOG("GameMap: Warning - Trying to convert coordinates with null tilemap");
        return Vec2::ZERO;
    }

    const  Size tileSize = _tileMap->getTileSize();//瓦片大小，本程序都是16*16
    const Size mapSize = _tileMap->getMapSize();//地图大小
    const  float scale = _tileMap->getScale();

    // X坐标：直接除以瓦片宽度和缩放
    const int x = worldPosition.x / (tileSize.width* scale);
    // Y坐标：从地图底部转换回Tiled的从上往下
    const int y = mapSize.height - worldPosition.y / (scale * tileSize.height);

    //CCLOG("坐标转换：世界坐标(%.1f, %.1f) -> 瓦片坐标(%d, %d)",worldPosition.x, worldPosition.y, x, y);

    return Vec2(x, y);
}

bool GameMap::checkForTransition(const Vec2& tilePos, TransitionInfo& outTransition) const {
    // 首先检查坐标是否在地图范围内
    const Size mapSize = _tileMap->getMapSize();
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

//参数为世界坐标
//本函数会将世界坐标改为地图坐标tilePos，判断当前地图坐标上是否有障碍物
bool GameMap::isWalkable(const Vec2& worldPos) const {
    if (!_tileMap) {
        //CCLOG("碰撞检测：地图未加载");
        return false;
    }

    const Vec2 tilePos = convertToTileCoord(worldPos);

    // 检查宝箱碰撞
    if (isChestCollision(worldPos)) {
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
                const int tileGID = layer->getTileGIDAt(tilePos);
                if (tileGID > 0) {
                    return false;  // 任何Buildings层有图块都不可通行
                }
            }
        }
    }
    // 检查坐标是否在地图范围内
    const Size mapSize = _tileMap->getMapSize();
    if (tilePos.x < 0 || tilePos.x >= mapSize.width ||
        tilePos.y < 0 || tilePos.y >= mapSize.height) {
        //CCLOG("碰撞检测：位置 (%.0f, %.0f) 超出地图范围", tilePos.x, tilePos.y);
        return false;
    }
   
    return true;  // 没有图块或没有Buildings层，则可以通行
}

bool GameMap::isChestCollision(const Vec2& worldPos) const
{
    if (!_tileMap) {
        CCLOG("No tilemap loaded");
        return false;
    }

    // 转换为瓦片坐标
    const Vec2 tilePos = convertToTileCoord(worldPos);

    // 获取宝箱碰撞层
    auto chestLayer = _tileMap->getLayer("CollisionChest");
    if (chestLayer) {
        // 检查该位置是否有碰撞图块
        const int tileGID = chestLayer->getTileGIDAt(tilePos);
        if (tileGID > 0) {
            CCLOG("Chest collision detected at tile position (%.1f, %.1f)",
                tilePos.x, tilePos.y);
            return true;
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

void GameMap::updateFrontTileVisibility(const Vec2& playerPos) {
    if (_frontLayers.empty()) return;

    const Vec2 currentTilePos = convertToTileCoord(playerPos);
    if (currentTilePos == _lastPlayerTilePos) return;

    // 恢复上一个位置的瓦片到完全不透明
    for (const auto& [layer, tilePos] : _fadedTiles) {
        const auto gid = layer->getTileGIDAt(tilePos);
        if (gid > 0) {
            auto tile = layer->getTileAt(tilePos);
            tile->stopAllActions();
            tile->runAction(FadeTo::create(TRANSITION_TIME, 255));
        }
    }

    _fadedTiles.clear();

    // 对每个Front层处理当前位置的瓦片
    const int radius = 1;
    for (auto* layer : _frontLayers) {
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dy = -radius; dy <= radius; dy++) {
                Vec2 checkPos(currentTilePos.x + dx, currentTilePos.y + dy);
                const auto gid = layer->getTileGIDAt(checkPos);
                if (gid > 0) {
                    auto tile = layer->getTileAt(checkPos);
                    tile->stopAllActions();
                    tile->runAction(FadeTo::create(TRANSITION_TIME, FRONT_TILE_OPACITY));
                    _fadedTiles.insert({ layer, checkPos });
                }
            }
        }
    }

    _lastPlayerTilePos = currentTilePos;
}
void GameMap::initFrontLayers() {
    if (!_tileMap) return;

    _frontLayers.clear();

    // 获取所有层
    const auto& allLayers = _tileMap->getChildren();
    for (const auto& child : allLayers) {
        auto layer = dynamic_cast<TMXLayer*>(child);
        if (layer) {
            // 获取层名
            std::string layerName = layer->getLayerName();
            // 检查是否以"Front"开头
            if (layerName.substr(0, 5) == "Front") {
                _frontLayers.push_back(layer);
                layer->retain();  // 保持引用计数
            }
        }
    }
}
void GameMap::restoreAllFrontTiles() {
    for (const auto& [layer, tilePos] : _fadedTiles) {
        const auto gid = layer->getTileGIDAt(tilePos);
        if (gid > 0) {
            auto tile = layer->getTileAt(tilePos);
            tile->stopAllActions();
            tile->setOpacity(255);
        }
    }

    _fadedTiles.clear();
    _lastPlayerTilePos = Vec2(-1, -1);
}
GameMap::~GameMap() {
    // 释放所有保持的Front层
    for (auto* layer : _frontLayers) {
        layer->release();
    }
    _frontLayers.clear();
}
void GameMap::refreshResources()
{
    if (_mapName != "Farm") return;

    auto backLayer = _tileMap->getLayer("Back");
    auto collisionLayer = _tileMap->getLayer("Collision");
    if (!backLayer || !collisionLayer)
    {
        CCLOG("Failed to refresh resources: layers not found");
        return;
    }

    const Size mapSize = _tileMap->getMapSize();
    for (int x = 0; x < mapSize.width; x++)
    {
        for (int y = 0; y < mapSize.height; y++)
        {
            const Vec2 tilePos(x, y);
            const int tileGID = backLayer->getTileGIDAt(tilePos);

            if (tileGID == RESOURCE_REMOVED_TILE_ID)
            {
                // 随机选择一个新的资源图块 ID
                const  int newResourceID = RESOURCE_TILES[rand() % RESOURCE_TILES.size()];

                // 更新资源层
                backLayer->setTileGID(newResourceID, tilePos);

                // 在碰撞层添加对应的碰撞图块
                // 假设碰撞图块的 ID 为 1，且已在 Tileset 中设置了 collision 属性
                collisionLayer->setTileGID(newResourceID, tilePos);
            }
        }
    }
}