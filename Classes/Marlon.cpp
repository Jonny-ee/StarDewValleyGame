#include "Marlon.h"

USING_NS_CC;

Marlon* Marlon::create()
{
    Marlon* npc = new (std::nothrow) Marlon();
    if (npc && npc->init())
    {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}

bool Marlon::init()
{
    const std::string imagePath = "Characters/Marlon.png"; // ���� NPC ��ͼƬ·��
    if (!NPC::init(imagePath))
    {
        return false;
    }
    this->setTextureRect(cocos2d::Rect(0, 0, 16, 32)); // ���óߴ�
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // ����ê��
    this->setScale(2.5);
    initializeDefaultBehavior();
    return true;
}

void Marlon::initializeDefaultBehavior()
{
    setName("����");
    setOccupation("��");
    setBirthday(Season::SUMMER, 10); // �����������ļ��ĵ�10��

    // ��ʼ�������б�
    taskList.push_back("�ɼ�5����ʯ");
    taskList.push_back("�ҵ�ϡ�п�ʯ");
    taskList.push_back("�����������");
}

void Marlon::assignTask(Player* player)
{
    // Ϊ��ҷ����������
    if (!taskList.empty()) {
        int randomIndex = rand() % taskList.size();
        std::string task = taskList[randomIndex];
        CCLOG("�����������: %s", task.c_str());

        // ���������Ӹ���������߼������������ҵ�����״̬
        // player->addTask(task); // ���� Player ���� addTask ����
    }
}

void Marlon::startConversation()
{
    // ��ʾ�Ի���
    std::string dialogue = getRandomDialogue();
    // DialogueManager::getInstance()->showDialogue(dialogue); // �����жԻ�������
    CCLOG("�Ի�: %s", dialogue.c_str());
}

std::string Marlon::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}