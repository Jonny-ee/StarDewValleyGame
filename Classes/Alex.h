#pragma once
#include "NPC.h"

// ������˹ NPC ��
class Alex : public NPC
{
public:
    static Alex* create();
    virtual bool init();

    void startConversation(); // ��ʼ�Ի�
    std::string getRandomDialogue();
    void initializeAnimations();
private:
    void initializeDefaultBehavior(); // ��ʼ��������˹ NPC ��Ĭ����Ϊ
    std::vector<std::string> dialogueList = {
      "Today is a great day to train!",
      "Hey, farmer! If you want to get stronger, you should do more sports.",
      "I like working out with my friends, especially Maru.",
      "Nice weather today. Perfect for getting a good workout in."
    };
};