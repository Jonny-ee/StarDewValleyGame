#include "NPC.h"

USING_NS_CC;

void NPC::addDialogue(std::string dialogue)
{
    dialogueList.push_back(dialogue);
}


std::string NPC::getRandomDialogue()
{
    int index = std::rand() % 4;
    return dialogueList[index];
}


void NPC::showFull()
{
    // 初始化表情
    auto emoji = Sprite::create("TileSheets/emotes.png");
    if (emoji) {
        emoji->setTextureRect(cocos2d::Rect(0, 80, 16, 16));
        emoji->setScale(1.0f);
        emoji->setAnchorPoint(Vec2(-1, -1));  // 设置emoji的锚点为左下角
        emoji->setVisible(true);
        this->addChild(emoji);
    }
    // 创建延迟动作，2秒后恢复状态
    auto delay = DelayTime::create(2.0f);
    auto resetAction = CallFunc::create([emoji]() {
        emoji->setVisible(false);  // 2秒后将emoji隐藏
        });
    // 将延迟动作和重置动作结合
    auto sequence = Sequence::create(delay, resetAction, nullptr);
    this->runAction(sequence); // 运行动画
}






