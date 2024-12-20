#pragma once
#include "cocos2d.h"
#include "ItemSystem.h"

USING_NS_CC;

// ����״̬ö��
enum class QuestState {
    NOT_STARTED,    // ����δ��ʼ
    IN_PROGRESS,    // ���������
    COMPLETED       // ���������
};

// ��������ö��
enum class QuestType {
    COLLECT_WOOD,   // �ռ�ľͷ����
    REPAIR_BRIDGE,  // ����������
    // ��Ӹ�����������...
};

// �������ݽṹ
struct QuestData {
    QuestType type;
    std::string title;
    std::string description;
    int targetAmount;
    int currentAmount;
    QuestState state;
};

class QuestSystem {
public:
    static QuestSystem* getInstance();

    // ��ʼ������ϵͳ
    void init();

    // ��ʼ����
    void startQuest(QuestType type);

    // �����������
    void updateQuestProgress(QuestType type, int amount);

    // �������
    void completeQuest(QuestType type);

    // ��ȡ����״̬
    QuestState getQuestState(QuestType type) const;

    // ��ȡ��������
    const QuestData& getQuestData(QuestType type) const;

    void resetQuest(QuestType type);
private:
    QuestSystem() {}
    static QuestSystem* _instance;
    std::map<QuestType, QuestData> _quests;
};