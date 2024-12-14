#include "Alex.h"

USING_NS_CC;

Alex* Alex::create()
{
    Alex* npc = new (std::nothrow) Alex();
    if (npc && npc->init())
    {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}

bool Alex::init()
{
    const std::string imagePath = "Characters/Alex.png"; // ������˹ NPC ��ͼƬ·��
    if (!NPC::init(imagePath))
    {
        return false;
    }
    this->setTextureRect(cocos2d::Rect(0, 128, 16, 32)); // ���óߴ�
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // ����ê��
    this->setScale(2.5);
    initializeDefaultBehavior();
    return true;
}

void Alex::initializeDefaultBehavior()
{
    setName("������˹");
    setOccupation("�˶�Ա");
    setBirthday(Season::SPRING, 8); // ���������ڴ����ĵ�8��

}

void Alex::startConversation()
{
    // ��ʾ�Ի���
    std::string dialogue = getRandomDialogue();
    // DialogueManager::getInstance()->showDialogue(dialogue); // �����жԻ�������
    CCLOG("�Ի�: %s", dialogue.c_str());
}

std::string Alex::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}

void Alex::initializeAnimations()
{
    // ��������
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.15f); // ÿ֡����ʱ��

    // �������8֡����
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 4; j++) {
            auto frame = SpriteFrame::create("Characters/Alex.png", cocos2d::Rect(j * 16, 128 + i * 32, 16, 32)); // ÿ֡������
            animation->addSpriteFrame(frame);
        }
    }

    // ����ѭ������
    auto runAnimation = RepeatForever::create(Animate::create(animation));
    this->runAction(runAnimation); // ���ж���
}