#pragma once
#include "NPC.h"

// ��³ NPC ��
class Maru : public NPC
{
public:
    static Maru* create();
    virtual bool init();

    void startConversation(); // ��ʼ�Ի�
    std::string getRandomDialogue();
private:
    void initializeDefaultBehavior(); // ��ʼ����³ NPC ��Ĭ����Ϊ
    std::vector<std::string> dialogueList = {
      "Welcome to Stardew Valley Hospital. Come to me at anytime.",
      "If you're sick, remember to drink plenty of water and eat healthily.",
      "Oh, you're injured? Let me check it out.",
      "While it's quiet, there are occasionally situations that need attention."
    };
};