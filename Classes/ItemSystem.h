#pragma once
#include "cocos2d.h"

/*
 * ��Ʒϵͳ��
 * ���ܣ�������Ϸ��������Ʒ�����ݺͲ���
 * 1. ��Ʒ����ɾ�Ĳ�
 * 2. ��Ʒ�����Ĺ���
 * 3. ��Ʒ��Ч����֤
 */
class ItemSystem
{
private:
    static ItemSystem* instance;                                // ����ʵ��ָ��
    std::map<std::string, int> inventory;                       // ��Ʒ�洢ӳ�����ƷID��������

    // ��Ч��ƷID�б�
    const std::set<std::string> validItems =
    {
        "wood",  "apple", "corn", "bread", "tomato", "fish", "stone"
    };

    // ����ģʽ���
    ItemSystem() {}                                             // ˽�й��캯��
    ~ItemSystem() {}                                            // ˽����������
    ItemSystem(const ItemSystem&) = delete;                     // ���ÿ������캯��
    ItemSystem& operator=(const ItemSystem&) = delete;          // ���ø�ֵ�����

    // ��Ʒ��֤
    bool isValidItem(const std::string& itemId) const;          // �����Ʒ�Ƿ���Ч

public:
    static ItemSystem* getInstance();                           // ��ȡ����ʵ��

    // ��Ʒ������ط���
    bool addItem(const std::string& itemId, int count);         // �����Ʒ
    bool removeItem(const std::string& itemId, int count);      // �Ƴ���Ʒ
    int getItemCount(const std::string& itemId);                // ��ȡ��Ʒ����
    bool useItem(const std::string& itemId);                    // ʹ����Ʒ
    bool hasEnoughItems(const std::string& itemId, int count);  // �����Ʒ�����Ƿ��㹻
};