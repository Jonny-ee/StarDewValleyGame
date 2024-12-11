#include "SkillSystem.h"

using namespace std;

SkillUI* SkillUI::_instance = nullptr;

SkillUI* SkillUI::create() {
    SkillUI* pRet = new(std::nothrow) SkillUI();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        _instance = pRet;
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

std::string SkillUI::getSkillName(SkillType type) {
    switch (type) {
        case SkillType::FARMING: return "Farming";
        case SkillType::MINING:  return "Mining";
        case SkillType::FISHING: return "Fishing";
        case SkillType::COOKING: return "Cooking";
        default: return "Unknown";
    }
}

void SkillUI::showLevelUpTip(SkillType type, int newLevel, float bonus) {
    // 创建英文提示文本
    std::string tipText = getSkillName(type) + " Level Up! " +
        "Now Level " + std::to_string(newLevel);

    auto tipLabel = Label::createWithSystemFont(tipText, "Arial", 20);
    if (tipLabel) {
        tipLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
        tipLabel->setScale(0.8f);
        this->addChild(tipLabel, 10);

        // 设置动画效果
        tipLabel->setOpacity(0);
        auto fadeIn = FadeIn::create(0.5f);
        auto delay = DelayTime::create(1.5f);
        auto fadeOut = FadeOut::create(0.5f);
        auto remove = RemoveSelf::create();

        tipLabel->runAction(Sequence::create(fadeIn, delay, fadeOut, remove, nullptr));
    }
}

void SkillUI::updateSkillDisplay(SkillType type, int level, int exp, int expNeeded) {
    if (levelLabels.find(type) != levelLabels.end()) {
        // 更新等级显示
        levelLabels[type]->setString("Level " + std::to_string(level));
    }

    if (bonusLabels.find(type) != bonusLabels.end()) {
        // 更新经验值显示
        std::string expText = "EXP: " + std::to_string(exp) + "/" + std::to_string(expNeeded);
        bonusLabels[type]->setString(expText);
    }
}
bool SkillUI::init() {
    if (!Node::init()) {
        return false;
    }

    // 创建背景图片
    auto background = Sprite::create("LooseSprites/JunimoNoteMobile.png");
    if (background) {
        background->setPosition(Vec2(130, 70));  // 调整背景位置
        background->setScale(0.3f);  // 缩小背景到 0.3 倍
        this->addChild(background);
    }

    // 初始化每个技能的UI元素
    const std::vector<SkillType> skills = {
        SkillType::FARMING, SkillType::MINING,
        SkillType::FISHING, SkillType::COOKING
    };

    float currentY = START_Y;  // 调整起始位置
    for (const auto& type : skills) {
        // 创建技能名称标签
        auto nameLabel = Label::createWithSystemFont(getSkillName(type), "Arial", 13);  // 调整字体大小
        if (nameLabel) {
            nameLabel->setPosition(Vec2(100, currentY));
            nameLabel->setScale(0.3f); 
            nameLabel->setColor(Color3B::BLACK);  // 设置文字颜色为黑色
            this->addChild(nameLabel);
        }

        // 创建等级标签
        auto levelLabel = Label::createWithSystemFont("Level 1", "Arial", 13);  // 调整字体大小
        if (levelLabel) {
            levelLabel->setPosition(Vec2(130, currentY));
            levelLabel->setScale(0.3f); 
            levelLabel->setColor(Color3B::BLACK);  // 设置文字颜色为黑色
            this->addChild(levelLabel);
            levelLabels[type] = levelLabel;
        }

        // 创建经验值标签
        auto expLabel = Label::createWithSystemFont("EXP: 0/100", "Arial", 13);  // 调整字体大小
        if (expLabel) {
            expLabel->setPosition(Vec2(160, currentY));
            expLabel->setScale(0.3f); 
            expLabel->setColor(Color3B::BLACK);  // 设置文字颜色为黑色
            this->addChild(expLabel);
            bonusLabels[type] = expLabel;
        }

        currentY -= SPACING_Y * 0.3f;  //间距
    }

    return true;
}