#pragma once
#include "NPC.h"

// 玛鲁 NPC 类
class Maru : public NPC
{
public:
    static Maru* create();
    virtual bool init();
    void staticAnimation() override;
private:
    void initializeDefaultBehavior(); // 初始化玛鲁 NPC 的默认行为
    void completeDialogue()
    {
        this->addDialogue("Welcome to Stardew Valley Hospital. Come to me at anytime.");
        this->addDialogue("If you're sick, remember to drink plenty of water and eat healthily.");
        this->addDialogue("Oh, you're injured? Let me check it out.");
        this->addDialogue("While it's quiet, there are occasionally situations that need attention.");
    }
};
