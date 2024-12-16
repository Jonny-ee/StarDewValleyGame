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
    const std::string imagePath = "Characters/Marlon.png"; // 马龙 NPC 的图片路径
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

void Marlon::initializeDefaultBehavior()
{
    setName("马龙");
    setOccupation("矿工");
    setBirthday(Season::SUMMER, 10); // 假设生日在夏季的第10天

    // 初始化任务列表
    taskList.push_back("采集5个矿石");
    taskList.push_back("找到稀有矿石");
    taskList.push_back("帮助我修理矿车");
}

void Marlon::assignTask(Player* player)
{
    // 为玩家分配随机任务
    if (!taskList.empty()) {
        int randomIndex = rand() % taskList.size();
        std::string task = taskList[randomIndex];
        CCLOG("任务分配给玩家: %s", task.c_str());

        // 这里可以添加更多的任务逻辑，例如更新玩家的任务状态
        // player->addTask(task); // 假设 Player 类有 addTask 方法
    }
}

void Marlon::startConversation()
{
    // 显示对话框
    std::string dialogue = getRandomDialogue();
    // DialogueManager::getInstance()->showDialogue(dialogue); // 假设有对话管理器
    CCLOG("对话: %s", dialogue.c_str());
}

std::string Marlon::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}