#pragma once
#include "cocos2d.h"

/*
 * 背包界面类
 * 功能：管理和显示玩家的背包界面
 * 包括：
 * 1.背包的显示和隐藏
 * 2.物品的增删改查
 * 3.物品数量的显示
 */
class InventoryUI : public cocos2d::Node
{
public:
    static InventoryUI* create();                               // 创建背包UI对象（增删物品种类的方法在此）
    virtual bool init() override;                               // 初始化背包UI

    void toggleVisibility();                                    // 切换背包显示状态

    // 物品操作相关方法
    void addItem(const std::string& itemId, int count = 1);     // 添加物品
    bool removeItem(const std::string& itemId, int count = 1);  // 移除物品
    int getItemCount(const std::string& itemId) const;          // 获取物品数量

private:
    void createUI();                                            // 创建UI界面

    cocos2d::LayerColor* _bgLayer;                              // 背景层
    std::unordered_map<std::string, int> _items;                // 物品数据（ID和数量）
    std::map<std::string, cocos2d::Label*> _countLabels;        // 数量标签映射表
};