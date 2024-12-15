#pragma once
#include "cocos2d.h"
#include "GameMap.h"
#include "Player.h"

USING_NS_CC;

/*
 * 农作物管理类
 * 功能：管理游戏中的农作系统
 * 1.土地开垦
 * 2.图块状态管理
 * 3.农作交互操作
 */
class CropManager
{
public:
    static CropManager* getInstance();                      // 获取单例实例

    bool canTill(const Vec2& tilePos) const;               // 检查指定位置是否可以开垦
    bool tillSoil(const Vec2& tilePos);                    // 开垦指定位置的土地

    bool canWater(const Vec2& tilePos) const;              // 检查指定位置是否可以浇水
    bool waterSoil(const Vec2& tilePos);                   // 浇水指定位置的土地

    void setGameMap(GameMap* map) { _gameMap = map; }      // 设置当前游戏地图引用
    void onMouseDown(const Vec2& mousePos, Player* player); // 处理鼠标事件

private:
    CropManager() {}                // 私有构造函数（单例模式）
    static CropManager* _instance;  // 单例实例指针
    GameMap* _gameMap = nullptr;    // 当前地图引用

    // 浇水效果相关常量
    const Color3B WATER_COLOR = Color3B(173, 216, 230);    // 浇水后的颜色(淡蓝色)
    const float WATER_DURATION = 1.0f;                      // 浇水效果持续时间
    const float FADE_DURATION = 0.3f;                       // 颜色恢复过渡时间

    /*
     * 图块ID说明：
     * 1. Tiled编辑器中图块ID从0开始，实际使用时需要加1
     * 2. 因为Tiled中0表示空图块，所以实际图块ID = Tiled中开垦后ID + 1
     * 3. 下面使用的都是ID都为实际使用的ID（已加1）
     */
    const std::vector<int> TILLABLE_TILES = { 207, 227, 228, 232, 464 };    // 可开垦的图块ID列表
    const int TILLED_TILE_ID = 681;                                         // 开垦后的图块ID（已开垦的农田图块）

    void createWaterEffect(Sprite* tile);                   // 创建浇水效果
    void showWateringPopup();                              // 显示浇水提示
};