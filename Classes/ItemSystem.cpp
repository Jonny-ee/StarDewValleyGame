#include "ItemSystem.h"

USING_NS_CC;

// ��ʼ����̬��Ա����
ItemSystem* ItemSystem::instance = nullptr;

/*
 * ��ȡ��Ʒϵͳʵ��
 * ���ܣ�ʵ�ֵ���ģʽ��ȷ��ȫ��ֻ��һ����Ʒϵͳʵ��
 * @return ������Ʒϵͳʵ����ָ��
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
 * �����Ʒ�Ƿ���Ч
 * ���ܣ���֤��ƷID�Ƿ���Ԥ�������Ч��Ʒ�б���
 * @param itemId ��ƷID
 * @return ��Ʒ��Ч����true����Ч����false
 */
bool ItemSystem::isValidItem(const std::string& itemId) const
{
    return validItems.find(itemId) != validItems.end();
}

/*
 * �����Ʒ
 * ���ܣ��򱳰������ָ����������Ʒ
 * @param itemId ��ƷID
 * @param count �������
 * @return ��ӳɹ�����true����Ʒ��Ч����false
 */
bool ItemSystem::addItem(const std::string& itemId, int count)
{
    if (!isValidItem(itemId))
    {
        CCLOG("Invalid item ID: %s", itemId.c_str());
        return false;
    }

    inventory[itemId] += count;
    markAsDiscovered(itemId);   // �����Ʒʱ���Ϊ�ѻ�ù�
    CCLOG("Added %d %s, current count: %d", count, itemId.c_str(), inventory[itemId]);

    // ���ø��»ص�
    if (_updateCallback)
    {
        _updateCallback();
    }

    return true;
}

/*
 * �Ƴ���Ʒ
 * ���ܣ��ӱ������Ƴ�ָ����������Ʒ
 * @param itemId ��ƷID
 * @param count �Ƴ�����
 * @return �����Ʒ��Ч�������㹻����true�����򷵻�false
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

        // ���ø��»ص�
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
 * ��ȡ��Ʒ����
 * ���ܣ���ȡ������ָ����Ʒ�ĵ�ǰ����
 * @param itemId ��ƷID
 * @return ������Ʒ�����������Ʒ��Ч�򲻴��ڷ���0
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
 * ʹ����Ʒ
 * ���ܣ�ʹ��һ��ָ������Ʒ������һ����
 * @param itemId ��ƷID
 * @return �����Ʒ��Ч��ʹ�óɹ�����true�����򷵻�false
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
 * �����Ʒ����
 * ���ܣ�����Ƿ����㹻������ָ����Ʒ
 * @param itemId ��ƷID
 * @param count ��Ҫ������
 * @return �����Ʒ��Ч�������㹻����true�����򷵻�false
 */
bool ItemSystem::hasEnoughItems(const std::string& itemId, int count)
{
    return getItemCount(itemId) >= count;   // ֱ��ʹ��getItemCount������ȡ�����Ա�
}