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
    const std::string imagePath = "Characters/Maru_Hospital.png"; // 玛鲁 NPC 的图片路径
    if (!NPC::init(imagePath))
    {
        return false;
    }
    this->setTextureRect(cocos2d::Rect(0, 0, 16, 32)); // 设置尺寸
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // 设置锚点
    this->setScale(2.5);
    initializeDefaultBehavior();
    return true;
}

void Maru::initializeDefaultBehavior()
{
    setName("玛鲁");
    setOccupation("护士");
    setBirthday(Season::WINTER, 10); // 假设生日在冬季的第10天

}

void Maru::startConversation()
{
    // 显示对话框
    std::string dialogue = getRandomDialogue();
    // DialogueManager::getInstance()->showDialogue(dialogue); // 假设有对话管理器
    CCLOG("对话: %s", dialogue.c_str());
}

std::string Maru::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}