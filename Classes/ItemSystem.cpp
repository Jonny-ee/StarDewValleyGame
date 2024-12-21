#include "ItemSystem.h"

USING_NS_CC;

// 初始化静态成员变量
ItemSystem* ItemSystem::instance = nullptr;

/*
 * 获取物品系统实例
 * 功能：实现单例模式，确保全局只有一个物品系统实例
 * @return 返回物品系统实例的指针
 */
ItemSystem* ItemSystem::getInstance()
{
    if (!instance)
    {
        instance = new ItemSystem();
    }
    return instance;
}

/*
 * 检查物品是否有效
 * 功能：验证物品ID是否在预定义的有效物品列表中
 * @param itemId 物品ID
 * @return 物品有效返回true，无效返回false
 */
bool ItemSystem::isValidItem(const std::string& itemId) const
{
    return validItems.find(itemId) != validItems.end();
}

/*
 * 添加物品
 * 功能：向背包中添加指定数量的物品
 * @param itemId 物品ID
 * @param count 添加数量
 * @return 添加成功返回true，物品无效返回false
 */
bool ItemSystem::addItem(const std::string& itemId, int count)
{
    if (!isValidItem(itemId))
    {
        CCLOG("Invalid item ID: %s", itemId.c_str());
        return false;
    }

    inventory[itemId] += count;
    markAsDiscovered(itemId);   // 添加物品时标记为已获得过
    CCLOG("Added %d %s, current count: %d", count, itemId.c_str(), inventory[itemId]);

    // 调用更新回调
    if (_updateCallback)
    {
        _updateCallback();
    }

    return true;
}

/*
 * 移除物品
 * 功能：从背包中移除指定数量的物品
 * @param itemId 物品ID
 * @param count 移除数量
 * @return 如果物品有效且数量足够返回true，否则返回false
 */
bool ItemSystem::removeItem(const std::string& itemId, int count)
{
    if (!isValidItem(itemId))
    {
        CCLOG("Invalid item ID: %s", itemId.c_str());
        return false;
    }

    if (inventory[itemId] >= count)
    {
        inventory[itemId] -= count;
        CCLOG("Removed %d %s, current count: %d", count, itemId.c_str(), inventory[itemId]);

        // 调用更新回调
        if (_updateCallback)
        {
            _updateCallback();
        }
        return true;
    }

    CCLOG("Not enough %s. Required: %d, Current: %d", itemId.c_str(), count, inventory[itemId]);
    return false;
}

/*
 * 获取物品数量
 * 功能：获取背包中指定物品的当前数量
 * @param itemId 物品ID
 * @return 返回物品数量，如果物品无效或不存在返回0
 */
int ItemSystem::getItemCount(const std::string& itemId)
{
    if (!isValidItem(itemId))
    {
        CCLOG("Invalid item ID: %s", itemId.c_str());
        return 0;
    }

    return inventory[itemId];
}

/*
 * 使用物品
 * 功能：使用一个指定的物品（消耗一个）
 * @param itemId 物品ID
 * @return 如果物品有效且使用成功返回true，否则返回false
 */
bool ItemSystem::useItem(const std::string& itemId)
{
    if (hasEnoughItems(itemId, 1))
    {
        CCLOG("Used 1 %s", itemId.c_str());
        return removeItem(itemId, 1);
    }
    CCLOG("Cannot use %s: item not available", itemId.c_str());
    return false;
}

/*
 * 检查物品数量
 * 功能：检查是否有足够数量的指定物品
 * @param itemId 物品ID
 * @param count 需要的数量
 * @return 如果物品有效且数量足够返回true，否则返回false
 */
bool ItemSystem::hasEnoughItems(const std::string& itemId, int count)
{
    return getItemCount(itemId) >= count;   // 直接使用getItemCount函数获取数量对比
}