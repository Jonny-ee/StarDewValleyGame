#pragma once
#include "NPC.h"

// 玛鲁 NPC 类
class Maru : public NPC
{
public:
    static Maru* create();
    virtual bool init();

    void startConversation(); // 开始对话
    std::string getRandomDialogue();
private:
    void initializeDefaultBehavior(); // 初始化玛鲁 NPC 的默认行为
    std::vector<std::string> dialogueList = {
      "Welcome to Stardew Valley Hospital. Come to me at anytime.",
      "If you're sick, remember to drink plenty of water and eat healthily.",
      "Oh, you're injured? Let me check it out.",
      "While it's quiet, there are occasionally situations that need attention."
    };
};