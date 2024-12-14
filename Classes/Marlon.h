#pragma once
#include "NPC.h"

// 马龙 NPC 类
class Marlon : public NPC
{
public:
    static Marlon* create();
    virtual bool init();

    void assignTask(Player* player); // 为玩家分配任务
    void startConversation(); // 开始对话
    std::string getRandomDialogue();
private:
    void initializeDefaultBehavior(); // 初始化马龙 NPC 的默认行为
    std::vector<std::string> taskList; // 任务列表
    std::vector<std::string> dialogueList = {
      "Can't resist the urge for adventure? Great, today's challenge is here.",
      "The light of courage shines in your eyes. How about a challenge?",
      "Though I can only see with one eye, I will not miss any lurking beast.",
      "Don't get too close to the cage..."
    };
};