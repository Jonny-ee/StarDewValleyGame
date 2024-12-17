#pragma once
#include "cocos2d.h"
#include "GameMap.h"
#include "Player.h"
#include "GameScene.h" 
#include "ItemSystem.h"
#include "Corn.h"

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

    bool canTill(const Vec2& tilePos) const;                // 检查指定位置是否可以开垦
    bool tillSoil(const Vec2& tilePos);                     // 开垦指定位置的土地

    bool canWater(const Vec2& tilePos) const;               // 检查指定位置是否可以浇水
    bool waterSoil(const Vec2& tilePos);                    // 浇水指定位置的土地

    void setGameMap(GameMap* map)                           // 设置当前游戏地图引用
    {
        // 如果已经有监听器，先移除
        if (_keyboardListener)
        {
            Director::getInstance()->getEventDispatcher()->removeEventListener(_keyboardListener);
            _keyboardListener = nullptr;
        }

        _gameMap = map;
        initKeyboardListener();
    }
    void onMouseDown(const Vec2& mousePos, Player* player); // 处理鼠标事件
    void setGameScene(GameScene* scene) { _gameScene = scene; } 

    // 新增种植相关方法
    bool canPlant(const Vec2& tilePos) const;               // 检查指定位置是否可以种植
    bool plantCorn(const Vec2& tilePos);                    // 在指定位置种植玉米
    void initKeyboardListener();                            // 初始化键盘监听

    // 保存和加载相关
    void saveCrops();
    void loadCrops();
    void clearCrops();                  // 切换地图时清理作物精灵

private:
    CropManager() {}                    // 私有构造函数（单例模式）              
    bool _isInitialized = false;        // 添加初始化标志

    static CropManager* _instance;      // 单例实例指针
    GameMap* _gameMap = nullptr;        // 当前地图引用
    GameScene* _gameScene = nullptr;    // 添加场景引用

    // 浇水效果相关常量
    const Color3B WATER_COLOR = Color3B(173, 216, 230);     // 浇水后的颜色(淡蓝色)
    const float WATER_DURATION = 1.0f;                      // 浇水效果持续时间
    const float FADE_DURATION = 0.3f;                       // 颜色恢复过渡时间

    // 新增成员
    // 作物位置偏移常量
    const float CROP_OFFSET_X = 21.0f;     // 作物水平偏移量
    const float CROP_OFFSET_Y = -14.0f;    // 作物垂直偏移量
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;
    // 玉米生长阶段的图块ID
    const std::vector<cocos2d::Rect> CORN_GROWTH_RECTS = {
        cocos2d::Rect(0, 0, 16, 16),    // 种子
        cocos2d::Rect(16, 0, 16, 16),   // 生长阶段1
        cocos2d::Rect(32, 0, 16, 16),   // 生长阶段2
        cocos2d::Rect(48, 0, 16, 16),   // 生长阶段3
        cocos2d::Rect(64, 0, 16, 16)    // 成熟
    };
    // 作物信息结构体
    struct CropInfo
    {
        Vec2 position;      // 种植位置
        Vec2 tilePos;       // 瓦片坐标
        int growthStage;    // 生长阶段
        std::string type;   // 作物类型("corn"等)
    };

    std::vector<CropInfo> _cropInfos;     // 存储所有作物信息
    std::vector<Crop*> _crops;            // 当前显示的作物精灵

    /*
     * 图块ID说明：
     * 1. Tiled编辑器中图块ID从0开始，实际使用时需要加1
     * 2. 因为Tiled中0表示空图块，所以实际图块ID = Tiled中开垦后ID + 1
     * 3. 下面使用的都是ID都为实际使用的ID（已加1）
     */
    const std::vector<int> TILLABLE_TILES = { 207, 227, 228, 232, 464 };    // 可开垦的图块ID列表
    const int TILLED_TILE_ID = 681;                                         // 开垦后的图块ID（已开垦的农田图块）

    void createWaterEffect(Sprite* tile);   // 创建浇水效果
    void showWateringPopup();               // 显示浇水提示
};