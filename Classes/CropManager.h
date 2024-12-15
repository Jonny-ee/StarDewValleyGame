#pragma once
#include "cocos2d.h"
#include "GameMap.h"
#include "Player.h"

USING_NS_CC;

class CropManager
{
public:
    static CropManager* getInstance();

    // 检查是否可以开垦
    bool canTill(const Vec2& tilePos) const;

    // 开垦土地
    bool tillSoil(const Vec2& tilePos);

    // 设置当前地图
    void setGameMap(GameMap* map) { _gameMap = map; }

    // 处理鼠标点击事件
    void onMouseDown(const Vec2& mousePos, Player* player);

private:
    CropManager() {}
    static CropManager* _instance;
    GameMap* _gameMap = nullptr;

    /*
    下面是关于图块集ID的说明：
    1.由于Tiled默认0是空图块，故素材图块从1开始，因此看到的ID需要加1才是实际ID;
    2.下面使用的ID已经加一
    */

    // 可开垦的图块ID列表
    const std::vector<int> TILLABLE_TILES = { 207, 227, 228, 232, 464 };

    // 开垦后的图块ID
    const int TILLED_TILE_ID = 681;
};