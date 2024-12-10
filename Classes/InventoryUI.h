#pragma once
#include "cocos2d.h"

/*
 * ����������
 * ���ܣ��������ʾ��ҵı�������
 * ������
 * 1.��������ʾ������
 * 2.��Ʒ����ɾ�Ĳ�
 * 3.��Ʒ��������ʾ
 */
class InventoryUI : public cocos2d::Node
{
public:
    static InventoryUI* create();                               // ��������UI������ɾ��Ʒ����ķ����ڴˣ�
    virtual bool init() override;                               // ��ʼ������UI

    void toggleVisibility();                                    // �л�������ʾ״̬

    // ��Ʒ������ط���
    void addItem(const std::string& itemId, int count = 1);     // �����Ʒ
    bool removeItem(const std::string& itemId, int count = 1);  // �Ƴ���Ʒ
    int getItemCount(const std::string& itemId) const;          // ��ȡ��Ʒ����

private:
    void createUI();                                            // ����UI����

    cocos2d::LayerColor* _bgLayer;                              // ������
    std::unordered_map<std::string, int> _items;                // ��Ʒ���ݣ�ID��������
    std::map<std::string, cocos2d::Label*> _countLabels;        // ������ǩӳ���
};