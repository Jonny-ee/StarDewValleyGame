#pragma once
#include "cocos2d.h"

/*
 * 背包界面类
 * 功能：负责显示玩家的背包内容
 * 包括：
 * 1.背包界面显示布局
 * 2.物品增删改查
 * 3.物品数量显示
 */
class InventoryUI : public cocos2d::Node
{
public:
    static InventoryUI* create();                               // 创建背包UI（增删物品种类相关的方法在此）
    virtual bool init() override;                               // 初始化背包UI

    void toggleVisibility();                                    // 切换背包显示状态
    void updateDisplay();                                       // 更新显示

    // 物品操作相关方法
    void addItem(const std::string& itemId, int count = 1);     // 添加物品
    bool removeItem(const std::string& itemId, int count = 1);  // 移除物品
    int getItemCount(const std::string& itemId) const;          // 获取物品数量

private:
    void createUI();                                            // 创建UI布局

    cocos2d::LayerColor* _bgLayer;                              // 背景层
    std::map<std::string, cocos2d::Label*> _countLabels;        // 数量标签映射
};