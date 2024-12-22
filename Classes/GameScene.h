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
#include "Tree.h"
#include "InventoryUI.h"
#include "FishingSystem.h"
#include "StatusUI.h"
#include "BaseEvent.h"
#include "ItemSystem.h"

// 前向声明
class Player;
class GameMap;
class Chest;
/*
 * 游戏场景类
 * 功能：管理游戏主场景的所有内容
 * 1.地图加载和显示
 * 2.玩家管理
 * 3.地图切换
 * 4.事件处理
 * 5.背包系统
 */
class GameScene : public cocos2d::Scene
{
public:
	// 核心函数声明
	static cocos2d::Scene* createScene();							// 创建并返回游戏场景实例
	virtual bool init();											// 初始化游戏场景
	CREATE_FUNC(GameScene);											// cocos2d场景创建宏
	virtual void update(float dt) override;							// 场景更新函数，每帧调用
	InventoryUI* getInventoryUI() const { return _inventoryUI; }	// 获取背包UI实例
	Player* getPlayer() const { return player; }					// 获取玩家实例

	// 地图切换相关
	void switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos);	// 切换到指定地图和位置

	// 相机控制
	void updateCamera();											// 更新相机位置跟随玩家
	void updateQuestUIPosition();									// 更新任务UI的位置

	// 时间更新相关
	void onDayChanged();											// 处理日期变化时的更新
	std::vector<Tree*> trees;										// 存储场景中所有树木的指针数组
	std::vector<Ore*> ores;											// 存储所有矿石的指针
	void showDialogue(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName, int heartPoints);
	void toggleMute(Ref* sender);									// 切换静音状态

private:
	// 场景对象
	Player* player = nullptr;								// 玩家实例
	GameMap* _gameMap = nullptr;							// 当前游戏地图
	Lewis* lewis = nullptr;									// Lewis NPC实例
	bool isLewisCreated = false;							// Lewis是否已创建的标志
	Marlon* marlon = nullptr;								// Marlon NPC实例
	bool isMarlonCreated = false;							// Marlon是否已创建的标志
	Maru* maru = nullptr;									// Maru NPC实例
	bool isMaruCreated = false;								// Maru是否已创建的标志
	Alex* alex = nullptr;									// Alex NPC实例
	bool isAlexCreated = false;								// Alex是否已创建的标志
	std::vector<Pig*> pigs;									// 存储所有猪的实例
	bool isPigCreated = false;								// 猪是否已创建的标志
	std::vector<Chicken*> chickens;							// 存储所有鸡的实例
	bool isChickenCreated = false;							// 鸡是否已创建的标志
	std::vector<Sheep*> sheeps;								// 存储所有羊的实例
	bool isSheepCreated = false;							// 羊是否已创建的标志
	bool isTreeCreated = false;								// 树木是否已创建的标志
	bool isOreCreated = false;								// 添加标志以跟踪矿石是否已经创建
	DialogueBox* dialogueBox = nullptr;						// 对话框实例
	cocos2d::TMXTiledMap* tileMap = nullptr;				// Tiled地图实例
	cocos2d::TMXObjectGroup* collisionsGroup = nullptr;		// 碰撞检测组
	InventoryUI* _inventoryUI = nullptr;					// 背包UI实例
	StatusUI* _statusUI;									// 状态UI实例
	cocos2d::Sprite* toolIcon = nullptr;					// 工具图标精灵
	void initToolIcon();									// 初始化工具图标
	void updateToolIcon();									// 更新工具图标显示
	Sprite* seedIcon = nullptr;								// 种子图标精灵
	void initSeedIcon();									// 初始化种子图标
	void updateSeedIcon();									// 更新种子图标显示
	void initLewis();										// 初始化Lewis NPC
	void initMarlon();										// 初始化Marlon NPC
	void initMaru();										// 初始化Maru NPC
	void initAlex();										// 初始化Alex NPC
	void initPig();											// 初始化猪
	void initChicken();										// 初始化鸡
	void initSheep();										// 初始化羊
	void initTree();										// 初始化树木
	void initOre();											// 初始化矿石

	// 创建动物、树木和宝箱的方法
	void createPig(const Vec2& initialPosition, const std::vector<Vec2>& path);		// 创建带路径的猪
	void createPig(const Vec2& initialPosition);									// 创建固定位置的猪
	void createChicken(const Vec2& initialPosition, const std::vector<Vec2>& path);	// 创建带路径的鸡
	void createChicken(const Vec2& initialPosition);								// 创建固定位置的鸡
	void createSheep(const Vec2& initialPosition, const std::vector<Vec2>& path);	// 创建带路径的羊
	void createSheep(const Vec2& initialPosition);									// 创建固定位置的羊
	void createTree(const Vec2& initialPosition);									// 创建树木
	void createOre(const Vec2& initialPosition);									// 创建矿石
	std::vector<Chest*> _chests;													// 存储所有宝箱的实例
	void initChests();																// 初始化宝箱
	void clearChests();																// 清理所有宝箱

	// 矿洞进入记录
	int lastMineEnterDay = 0;										// 上次进入矿洞的日期
	int lastMineEnterMonth = 0;										// 上次进入矿洞的月份
	int lastMineEnterYear = 0;										// 上次进入矿洞的年份
	cocos2d::MenuItemImage* muteButton;								// 静音按钮
	bool isMuted;													// 当前静音状态
	std::vector<BaseEvent*> _events;								// 存储所有事件实例

	// 输入控制
	std::set<char> _pressedKeys;                                    // 当前按下的按键集合
	cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;    // 键盘事件监听器
	void initMouseListener();                                       // 初始化鼠标事件监听器

	// 任务系统UI
	Label* _questTipLabel = nullptr;								// 任务提示文本
	Sprite* _questMark = nullptr;									// 任务标记图标
	void updateQuestUI();											// 更新任务UI显示
	void showQuestMark(Node* target);								// 显示任务标记
	void hideQuestMark(Node* target);								// 隐藏任务标记
	void handleQuestDialogue(Lewis* lewis);							// 处理与Lewis的任务对话
	void handleWoodQuest(Lewis* lewis, QuestState questState);		// 处理木材收集任务
	void handleBridgeQuest(Lewis* lewis, QuestState questState);	// 处理桥梁修复任务
	void checkQuestProgress();										// 检查任务进度
};
