#pragma once
#include "NPC.h"

// 马龙 NPC 类
class Marlon : public NPC
{
public:
    static Marlon* create();
    virtual bool init() override;

    void staticAnimation() override;
private:
    void initializeDefaultBehavior(); // 初始化马龙 NPC 的默认行为
    void completeDialogue()
    {
        this->addDialogue("Can't resist the urge for adventure? Great, today's challenge is here.");
        this->addDialogue("The light of courage shines in your eyes. How about a challenge?");
        this->addDialogue("Though I can only see with one eye, I will not miss any lurking beast.");
        this->addDialogue("Don't get too close to the cage...");
    }
};
