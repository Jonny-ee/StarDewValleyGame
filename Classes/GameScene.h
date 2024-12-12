#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "GameMap.h"
#include "Lewis.h"
#include "InventoryUI.h"
#include "Chest.h"

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

    // 地图切换相关
    void switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);   // 切换到指定地图

    // 相机控制
    void updateCamera();                    // 更新相机位置


private:
    // 场景对象
    Player* player = nullptr;                             // 玩家对象指针
    GameMap* _gameMap = nullptr;                          // 游戏地图指针
    Lewis* lewis = nullptr;                               // Lewis指针
    DialogueBox* dialogueBox = nullptr;                   // 对话框指针
    cocos2d::TMXTiledMap* tileMap = nullptr;              // Tiled 地图系统
    cocos2d::TMXObjectGroup* collisionsGroup = nullptr;   // 碰撞组
    InventoryUI* _inventoryUI = nullptr;                  // 背包UI成员变量
    cocos2d::Sprite* toolIcon = nullptr;  // 工具图标
    void initToolIcon();                  // 初始化工具图标
    void updateToolIcon();                // 更新工具图标
    std::vector<Chest*> _chests;     // 存储所有宝箱
    void initChests();               // 初始化宝箱

    // 输入状态
    std::set<char> _pressedKeys;                                    // 已按下的按键集合
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;    // 键盘监听器
    void initMouseListener();                                       // 声明鼠标监听器初始化方法(NPC使用)
};
