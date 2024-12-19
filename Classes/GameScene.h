#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"
#include "Lewis.h"
#include "Marlon.h"
#include "Maru.h"
#include "Alex.h"
#include "Pig.h"
#include "Chicken.h"
#include "Sheep.h"
#include "InventoryUI.h"
#include "FishingSystem.h"
#include "StatusUI.h"
#include "BaseEvent.h"
#include "ItemSystem.h"

// 添加前向声明
class Player;
class GameMap;
class Chest;  // 添加这行

/*
 * 游戏场景类
 * 功能：管理游戏主场景的所有内容
 * 1. 地图加载和显示
 * 2. 玩家管理
 * 3. 地图切换
 * 4. 事件处理
 * 5. 背包系统
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();   // 创建场景
    virtual bool init();                    // 初始化场景
    CREATE_FUNC(GameScene);                 // 场景创建宏
    virtual void update(float dt) override;
    InventoryUI* getInventoryUI() const { return _inventoryUI; }    // 返回背包UI（给Player的按键判断使用）
    Player* getPlayer() const { return player; }

    // 地图切换相关
    void switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);   // 切换到指定地图

    // 相机控制
    void updateCamera();                    // 更新相机位置

    // 时间更新相关
    void onDayChanged();                    // 每日更新函数声明

private:
    // 场景对象
    Player* player = nullptr;                             // 玩家对象指针
    GameMap* _gameMap = nullptr;                          // 游戏地图指针
    Lewis* lewis = nullptr;                               // Lewis指针
    bool isLewisCreated = false;                          // 添加标志以跟踪刘易斯是否已创建
    Marlon* marlon = nullptr;                             // Marlon指针
    bool isMarlonCreated = false;                          // 添加标志以跟踪马龙是否已创建
    Maru* maru = nullptr;                                 // Maru指针
    bool isMaruCreated = false;                             // 添加标志以跟踪玛鲁是否已创建
    Alex* alex = nullptr;                                 // Alex指针
    bool isAlexCreated = false;                             // 添加标志以跟踪艾利克斯是否已创建
    std::vector<Pig*> pigs;                               // 存储所有猪的指针
    bool isPigCreated = false;                            // 添加标志以跟踪猪是否已经创建
    std::vector<Chicken*> chickens;                        // 存储所有鸡的指针
    bool isChickenCreated = false;                         // 添加标志以跟踪鸡是否已经创建
    std::vector<Sheep*> sheeps;                        // 存储所有羊的指针
    bool isSheepCreated = false;                         // 添加标志以跟踪羊是否已经创建
    DialogueBox* dialogueBox = nullptr;                   // 对话框指针
    cocos2d::TMXTiledMap* tileMap = nullptr;              // Tiled 地图系统
    cocos2d::TMXObjectGroup* collisionsGroup = nullptr;   // 碰撞组
    InventoryUI* _inventoryUI = nullptr;                  // 背包UI成员变量
    StatusUI* _statusUI;

    cocos2d::Sprite* toolIcon = nullptr;    // 工具图标
    void initToolIcon();                    // 初始化工具图标
    void updateToolIcon();                  // 更新工具图标
    Sprite* seedIcon = nullptr;             // 种子图标
    void initSeedIcon();                    // 初始化种子图标
    void updateSeedIcon();                  // 更新种子图标

    void initLewis();                //初始化Lewis
    void initMarlon();            // 初始化马龙
    void initMaru();            // 初始化玛鲁
    void initAlex();            // 初始化艾利克斯
    void initPig();                 // 初始化猪
    void initChicken();                 // 初始化鸡
    void initSheep();                   // 初始化羊
    void createPig(const Vec2& initialPosition, const std::vector<Vec2>& path);
    void createPig(const Vec2& initialPosition);
    void createChicken(const Vec2& initialPosition, const std::vector<Vec2>& path);
    void createChicken(const Vec2& initialPosition);
    void createSheep(const Vec2& initialPosition, const std::vector<Vec2>& path);
    void createSheep(const Vec2& initialPosition);
    std::vector<Chest*> _chests;     // 存储所有宝箱
    void initChests();               // 初始化宝箱
    void clearChests();              // 清理宝箱
    int lastMineEnterDay = 0;    // 记录上次进入矿洞的日期
    int lastMineEnterMonth = 0;  // 记录上次进入矿洞的月份
    int lastMineEnterYear = 0;   // 记录上次进入矿洞的年份
    std::vector<BaseEvent*> _events;
    // 输入状态
    std::set<char> _pressedKeys;                                    // 已按下的按键集合
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;    // 键盘监听器
    void initMouseListener();                                       // 声明鼠标监听器初始化方法(NPC使用)

};
