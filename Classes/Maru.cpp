#include "Maru.h"

USING_NS_CC;

Maru* Maru::create()
{
    Maru* npc = new (std::nothrow) Maru();
    if (npc && npc->init())
    {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}

bool Maru::init()
{
    const std::string imagePath = "Characters/Maru_Hospital.png"; // ��³ NPC ��ͼƬ·��
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

void Maru::initializeDefaultBehavior()
{
    setName("��³");
    setOccupation("��ʿ");
    setBirthday(Season::WINTER, 10); // ���������ڶ����ĵ�10��

}

void Maru::startConversation()
{
    // ��ʾ�Ի���
    std::string dialogue = getRandomDialogue();
    // DialogueManager::getInstance()->showDialogue(dialogue); // �����жԻ�������
    CCLOG("�Ի�: %s", dialogue.c_str());
}

std::string Maru::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}