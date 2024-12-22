#pragma once
#include "cocos2d.h"
#include "ItemSystem.h"
#include "GameScene.h"
#include "GameTime.h"

/*
 * 宝箱类
 * 功能：实现游戏中的宝箱系统
 * 1. 在地图上显示宝箱精灵
 * 2. 处理宝箱的开启状态
 */

class Chest : public cocos2d::Sprite
{
public:
    // 创建宝箱的静态方法
    static Chest* create();

    // 初始化方法
    virtual bool init();

    // 获取宝箱状态
    bool isOpened() const { return opened; }

    // 设置宝箱位置
    void setChestPosition(const cocos2d::Vec2& position);

    // 初始化触摸事件
    void initTouchEvents();

    // 检查玩家是否在可交互范围内
    bool isPlayerInRange(const cocos2d::Vec2& playerPos) const;

private:
    // 宝箱开启状态
    bool opened;     

    // 精灵表相关常量
    const std::string CHEST_SPRITE_FILE = "Chest.png";
    const int FRAME_WIDTH = 48;                    // 每帧宽度
    const int FRAME_HEIGHT = 48;                   // 每帧高度
    const int TOTAL_FRAMES = 5;                    // 总帧数
    const float ANIMATION_SPEED = 0.2f;            // 每帧动画的时间间隔
    const float INTERACTION_RANGE = 100.0f;        // 玩家可以开启宝箱的距离

    // 宝箱内物品相关配置
    const std::vector<std::string> possibleItems = {
        "wood", "apple", "corn", "bread",
        "tomato", "fish", "stone", "corn seed",
        "tomato seed"
    };

    const int MIN_ITEMS = 1;    // 宝箱最少物品数量
    const int MAX_ITEMS = 3;    // 宝箱最多物品数量


    // 记录上次开启的时间
    int lastOpenDay = 0;    // 上次开启的日期
    int lastOpenMonth = 0;  // 上次开启的月份
    int lastOpenYear = 0;   // 上次开启的年份

    // 检查是否可以重新开启
    bool canReopen() const;
    // 重置宝箱状态
    void resetChest();
    // 记录开启时间
    void recordOpenTime();

    void openChest();                           // 开启宝箱
    void playOpenAnimation();                   // 播放开启动画
    void onOpenAnimationFinished();             // 动画完成回调
    std::string generateRandomItem() const;     // 生成随机物品
    cocos2d::Animation* createChestAnimation(); // 创建宝箱动画

    // 物品获得信息结构体
    struct ItemInfo {
        std::string itemId;
        int quantity;
    };

    std::vector<ItemInfo> _obtainedItems; // 存储获得的所有物品
    void showItemsSummaryPopup();         // 显示物品汇总弹窗
};
