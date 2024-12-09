#pragma once
#include "cocos2d.h"

/*
 * 物品系统类
 * 功能：管理游戏中所有物品的数据和操作
 * 1. 物品的增删改查
 * 2. 物品数量的管理
 * 3. 物品有效性验证
 */
class ItemSystem
{
private:
    static ItemSystem* instance;                                // 单例实例指针
    std::map<std::string, int> inventory;                       // 物品存储映射表（物品ID，数量）

    // 有效物品ID列表
    const std::set<std::string> validItems =
    {
        "wood",  "apple", "corn", "bread", "tomato", "fish", "stone"
    };

    // 单例模式相关
    ItemSystem() {}                                             // 私有构造函数
    ~ItemSystem() {}                                            // 私有析构函数
    ItemSystem(const ItemSystem&) = delete;                     // 禁用拷贝构造函数
    ItemSystem& operator=(const ItemSystem&) = delete;          // 禁用赋值运算符

    // 物品验证
    bool isValidItem(const std::string& itemId) const;          // 检查物品是否有效

public:
    static ItemSystem* getInstance();                           // 获取单例实例

    // 物品操作相关方法
    bool addItem(const std::string& itemId, int count);         // 添加物品
    bool removeItem(const std::string& itemId, int count);      // 移除物品
    int getItemCount(const std::string& itemId);                // 获取物品数量
    bool useItem(const std::string& itemId);                    // 使用物品
    bool hasEnoughItems(const std::string& itemId, int count);  // 检查物品数量是否足够
};