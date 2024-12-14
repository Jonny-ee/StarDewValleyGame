#pragma once
#include "NPC.h"

// ���� NPC ��
class Marlon : public NPC
{
public:
    static Marlon* create();
    virtual bool init();

    void assignTask(Player* player); // Ϊ��ҷ�������
    void startConversation(); // ��ʼ�Ի�
    std::string getRandomDialogue();
private:
    void initializeDefaultBehavior(); // ��ʼ������ NPC ��Ĭ����Ϊ
    std::vector<std::string> taskList; // �����б�
    std::vector<std::string> dialogueList = {
      "Can't resist the urge for adventure? Great, today's challenge is here.",
      "The light of courage shines in your eyes. How about a challenge?",
      "Though I can only see with one eye, I will not miss any lurking beast.",
      "Don't get too close to the cage..."
    };
};