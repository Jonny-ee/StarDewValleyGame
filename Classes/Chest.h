#pragma once
#include "cocos2d.h"
#include "ItemSystem.h"

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

private:
    bool opened;     // 宝箱开启状态

    // 精灵表相关常量
    const std::string CHEST_SPRITE_FILE = "Chest.png";
    const int FRAME_WIDTH = 32;        // 每帧宽度
    const int FRAME_HEIGHT = 32;       // 每帧高度
    const int TOTAL_FRAMES = 10;       // 总帧数
    const float ANIMATION_SPEED = 0.08f; // 动画速度（每帧间隔）

    // 宝箱内物品相关配置
    const std::vector<std::string> possibleItems = {
        "wood", "apple", "corn", "bread",
        "tomato", "fish", "stone"
    };

    const int MIN_ITEMS = 1;    // 宝箱最少物品数量
    const int MAX_ITEMS = 3;    // 宝箱最多物品数量

    // 私有方法
    void openChest();                          // 开启宝箱
    void playOpenAnimation();                  // 播放开启动画
    void onOpenAnimationFinished();            // 动画完成回调
    std::string generateRandomItem() const;     // 生成随机物品
    cocos2d::Animation* createChestAnimation(); // 创建宝箱动画
    void createItemObtainEffect(const std::string& itemId, int index);
};