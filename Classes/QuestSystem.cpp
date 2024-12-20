#include "QuestSystem.h"

QuestSystem* QuestSystem::_instance = nullptr;

QuestSystem* QuestSystem::getInstance() {
    if (!_instance) {
        _instance = new QuestSystem();
        _instance->init();
    }
    return _instance;
}

void QuestSystem::init() {
    // 初始化木头收集任务
    QuestData woodQuest;
    woodQuest.type = QuestType::COLLECT_WOOD;
    woodQuest.title = "Collect Woods";
    woodQuest.description = "Can you help me collect 10 woods to repair the bridge?";
    woodQuest.targetAmount = 10;
    woodQuest.currentAmount = 0;
    woodQuest.state = QuestState::NOT_STARTED;
    // 初始化修桥任务
    QuestData bridgeQuest;
    bridgeQuest.type = QuestType::REPAIR_BRIDGE;
    bridgeQuest.title = "Repair the Bridge";
    bridgeQuest.description = "The bridge is broken, can you use woods to repair them?";
    bridgeQuest.targetAmount = 1;
    bridgeQuest.currentAmount = 0;
    bridgeQuest.state = QuestState::NOT_STARTED;

    _quests[QuestType::COLLECT_WOOD] = woodQuest;
    _quests[QuestType::REPAIR_BRIDGE] = bridgeQuest;
}

void QuestSystem::startQuest(QuestType type) {
    if (_quests.find(type) != _quests.end()) {
        _quests[type].state = QuestState::IN_PROGRESS;
    }
}

void QuestSystem::updateQuestProgress(QuestType type, int amount) {
    if (_quests.find(type) != _quests.end()) {
        auto& quest = _quests[type];
        if (quest.state == QuestState::IN_PROGRESS) {
            quest.currentAmount = amount;
        }
    }
}

void QuestSystem::completeQuest(QuestType type) {
    if (_quests.find(type) != _quests.end()) {
        auto& quest = _quests[type];
        quest.state = QuestState::COMPLETED;
    }
}

QuestState QuestSystem::getQuestState(QuestType type) const {
    if (_quests.find(type) != _quests.end()) {
        return _quests.at(type).state;
    }
    return QuestState::NOT_STARTED;
}

const QuestData& QuestSystem::getQuestData(QuestType type) const {
    return _quests.at(type);
}

void QuestSystem::resetQuest(QuestType type) {
    if (_quests.find(type) != _quests.end()) {
        auto& quest = _quests[type];
        quest.state = QuestState::NOT_STARTED;
        quest.currentAmount = 0;
    }
}