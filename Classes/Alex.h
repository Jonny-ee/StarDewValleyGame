#pragma once
#include "NPC.h"

// 艾利克斯 NPC 类
class Alex : public NPC
{
public:
    static Alex* create();
    virtual bool init();

    void startConversation(); // 开始对话
    std::string getRandomDialogue();
    void initializeAnimations();
private:
    void initializeDefaultBehavior(); // 初始化艾利克斯 NPC 的默认行为
    std::vector<std::string> dialogueList = {
      "Today is a great day to train!",
      "Hey, farmer! If you want to get stronger, you should do more sports.",
      "I like working out with my friends, especially Maru.",
      "Nice weather today. Perfect for getting a good workout in."
    };
};