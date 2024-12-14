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
    const std::string imagePath = "Characters/Alex.png"; // 艾利克斯 NPC 的图片路径
    if (!NPC::init(imagePath))
    {
        return false;
    }
    this->setTextureRect(cocos2d::Rect(0, 128, 16, 32)); // 设置尺寸
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // 设置锚点
    this->setScale(2.5);
    initializeDefaultBehavior();
    return true;
}

void Alex::initializeDefaultBehavior()
{
    setName("艾利克斯");
    setOccupation("运动员");
    setBirthday(Season::SPRING, 8); // 假设生日在春季的第8天

}

void Alex::startConversation()
{
    // 显示对话框
    std::string dialogue = getRandomDialogue();
    // DialogueManager::getInstance()->showDialogue(dialogue); // 假设有对话管理器
    CCLOG("对话: %s", dialogue.c_str());
}

std::string Alex::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}

void Alex::initializeAnimations()
{
    // 创建动画
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.15f); // 每帧持续时间

    // 精灵表有8帧动画
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 4; j++) {
            auto frame = SpriteFrame::create("Characters/Alex.png", cocos2d::Rect(j * 16, 128 + i * 32, 16, 32)); // 每帧的区域
            animation->addSpriteFrame(frame);
        }
    }

    // 创建循环动画
    auto runAnimation = RepeatForever::create(Animate::create(animation));
    this->runAction(runAnimation); // 运行动画
}