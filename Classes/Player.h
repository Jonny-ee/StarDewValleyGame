#pragma once
#include "cocos2d.h"
#include "SkillSystem.h"
/*
 * 玩家类
 * 功能：管理玩家角色的所有行为和属性
 * 包括：
 * 1. 玩家的移动控制
 * 2. 碰撞检测
 * 3. 键盘输入处理
 * 4. 动画状态管理
 */

class GameScene;    // 前向声明
class GameMap;  // 前向声明
class SkillUI;  // 前向声明
class Player : public cocos2d::Sprite
{

public:
    // 工具类型枚举
    enum class ToolType {
        NONE,       // 无工具
        SHOVEL,     // 铲子
        AXE,        // 斧头
        WATERING,   // 水壶
        ROD,        // 鱼竿
        GIFT        // 礼物(送给NPC)
    };
    ToolType getCurrentTool() const { return currentTool; }
    void switchTool();                                  // 切换工具的方法



    // 基础功能
    static Player* create();                            // 创建玩家对象
    virtual bool init();                                // 初始化玩家对象
    void moveInDirection(cocos2d::Vec2 direction);      // 移动玩家
    float getMoveSpeed() const;                         // 获取玩家移动速度
    void removeAllListeners();                          // 清理方法声明
    void setGameMap(GameMap* map) { gameMap = map; }    // 设置方法
    static Player* getInstance() { return _instance; }// 添加静态实例获取方法
    // 事件监听相关
    void initKeyboardListener();                        // 初始化键盘监听器
    void initMouseListener();                           // 初始化鼠标监听器
    void update(float dt);                              // 更新玩家状态

    // 碰撞检测相关
    void setCollisionGroup(cocos2d::TMXObjectGroup* group); // 设置碰撞检测组

    // 动作相关
    void performAction(const cocos2d::Vec2& clickPos);      // 执行动作
    void setCurrentTool(ToolType tool);                     // 设置当前工具

    // 技能等级系统相关
    void toggleSkillUI();  // 切换技能界面显示状态

    // 设置玩家是否可执行动作
    void setCanPerformAction(bool select) { canPerformAction = select; }
  
private:
    GameMap* gameMap = nullptr;                             // GameMap 引用                  
    static Player* _instance;
    // 基础属性
    float moveSpeed = 200.0f;                               // 玩家移动速度（像素/秒）

    // 碰撞检测相关
    cocos2d::TMXObjectGroup* collisionsGroup = nullptr;                                             // 碰撞检测组指针
    std::map<cocos2d::EventKeyboard::KeyCode, bool> keys;                                           // 按键状态映射表
    bool isKeyPressed(cocos2d::EventKeyboard::KeyCode code);                                        // 检查按键是否被按下
    bool checkCollision(const cocos2d::Vec2& nextPosition);                                         // 检测指定位置是否发生碰撞
    bool isPointInPolygon(const cocos2d::Vec2& point, const std::vector<cocos2d::Vec2>& vertices);  // 判断点是否在碰撞层多边形内

    // 动画相关
    int currentDirection = 0;                   // 当前朝向(0:下, 1:上, 2:左, 3:右)
    int currentFrame = 0;                       // 当前帧索引
    float animationTimer = 0;                   // 动画计时器
    const float FRAME_INTERVAL = 0.3f;          // 帧间隔时间

    // 动作相关
    cocos2d::Sprite* actionSprite = nullptr;    // 动作精灵
    bool isActioning = false;                   // 是否正在执行动作
    bool canPerformAction = true;               // 是否允许执行玩家动作，防止与NPC鼠标点击冲突
    float actionTimer = 0;                      // 动作计时器
    const float ACTION_DURATION = 0.4f;         // 动作持续时间
    void updateAction(float dt);                // 动作动画

    // 工具相关
    ToolType currentTool = ToolType::NONE;      // 当前装备的工具（初始化默认空手）
    cocos2d::Sprite* rodSprite = nullptr;           // 鱼竿精灵
    cocos2d::Sprite* gift = nullptr;                // 礼物精灵

    //技能等级系统相关
    bool isSkillUIVisible{ false };  // 技能界面显示状态
    SkillUI* skillUI{ nullptr };     // 技能界面指针
};
