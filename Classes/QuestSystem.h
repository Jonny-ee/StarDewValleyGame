#pragma once
#include "cocos2d.h"
#include "ItemSystem.h"

USING_NS_CC;

// 任务状态枚举
enum class QuestState {
    NOT_STARTED,    // 任务未开始
    IN_PROGRESS,    // 任务进行中
    COMPLETED       // 任务已完成
};

// 任务类型枚举
enum class QuestType {
    COLLECT_WOOD,   // 收集木头任务
    REPAIR_BRIDGE,  // 修理桥任务
    // 添加更多任务类型...
};

// 任务数据结构
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

    // 初始化任务系统
    void init();

    // 开始任务
    void startQuest(QuestType type);

    // 更新任务进度
    void updateQuestProgress(QuestType type, int amount);

    // 完成任务
    void completeQuest(QuestType type);

    // 获取任务状态
    QuestState getQuestState(QuestType type) const;

    // 获取任务数据
    const QuestData& getQuestData(QuestType type) const;

    void resetQuest(QuestType type);
private:
    QuestSystem() {}
    static QuestSystem* _instance;
    std::map<QuestType, QuestData> _quests;
};