#pragma once
#include "cocos2d.h"
#include <functional>

// ��ӻص��������Ͷ���
using UpdateCallback = std::function<void()>;

/*
 * ��Ʒϵͳ��
 * ���ܣ�������Ϸ��������Ʒ�����ݺͲ���
 * 1. ��Ʒ��ɾ�Ĳ�
 * 2. ��Ʒ�����Ĺ���
 * 3. ��Ʒ��Ч����֤
 */
class ItemSystem
{
private:
    static ItemSystem* instance;                                // ����ʵ��ָ��
    std::map<std::string, int> inventory;                       // ��Ʒ�洢ӳ�䣨��ƷID��������
    std::set<std::string> _discoveredItems;                     // ��¼�ѷ��ֵ���Ʒ
    // ��Ч��ƷID�б�
    const std::set<std::string> validItems =
    {
        "wood", "apple", "CatFood", "corn", "bread", "tomato", "fish", "mermaid's KISS(*)", "stone", "corn seed", "tomato seed"
    };

    // ����ģʽ���
    ItemSystem() {}                                             // ˽�й��캯��
    ~ItemSystem() {}                                            // ˽����������
    ItemSystem(const ItemSystem&) = delete;                     // ���ÿ������캯��
    ItemSystem& operator=(const ItemSystem&) = delete;          // ���ø�ֵ�����

    // ��Ʒ��֤
    bool isValidItem(const std::string& itemId) const;          // �����Ʒ�Ƿ���Ч

    // UI���»ص�
    UpdateCallback _updateCallback = nullptr;

public:
    static ItemSystem* getInstance();                           // ��ȡ����ʵ��

    // ��Ʒ������ط���
    bool addItem(const std::string& itemId, int count);         // �����Ʒ
    bool removeItem(const std::string& itemId, int count);      // �Ƴ���Ʒ
    int getItemCount(const std::string& itemId);                // ��ȡ��Ʒ����
    bool useItem(const std::string& itemId);                    // ʹ����Ʒ
    bool hasEnoughItems(const std::string& itemId, int count);  // �����Ʒ�����Ƿ��㹻
    bool hasDiscovered(const std::string& itemId) const { return _discoveredItems.count(itemId) > 0; }  // �Ƿ��ù���Ʒ
    void markAsDiscovered(const std::string& itemId) { _discoveredItems.insert(itemId); }               // ���Ϊ�ѻ�ù�
    // ����UI���»ص�
    void setUpdateCallback(UpdateCallback callback) { _updateCallback = callback; }
};