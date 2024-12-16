#include "SkillSystem.h"

using namespace std;

/*
 * 技能UI系统
 * 功能：显示和管理游戏中的技能界面
 */

SkillUI* SkillUI::_instance = nullptr;

/*
 * 创建技能UI实例
 * @return 返回技能UI对象指针，创建失败返回nullptr
 */
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

/*
 * 获取技能名称
 * @param type 技能类型
 * @return 返回对应技能的名称字符串
 */
std::string SkillUI::getSkillName(SkillType type) {
    switch (type) {
        case SkillType::FARMING: return "Farming";
        case SkillType::MINING:  return "Mining";
        case SkillType::FISHING: return "Fishing";
        case SkillType::COOKING: return "Cooking";
        default: return "Unknown";
    }
}

/*
 * 显示升级提示
 * @param type 升级的技能类型
 * @param newLevel 新的等级
 * @param bonus 技能加成值
 * 功能：创建并显示技能升级的动画提示
 */
void SkillUI::showLevelUpTip(SkillType type, int newLevel, float bonus) {
    // 获取玩家位置
    auto player = Player::getInstance();
    if (!player) return;

    Vec2 playerPos = player->getPosition();

    // 创建半透明黑色背景层
    auto bgLayer = LayerColor::create(Color4B(0, 0, 0, 180), 300, 150);

    // 创建提示文本
    std::string skillName = getSkillName(type);
    std::string bonusText;

    // 根据不同技能类型显示不同的加成说明
    switch (type) {
        case SkillType::FARMING:
            bonusText = StringUtils::format("Harvest bonus: +%.0f%%", (bonus - 1.0f) * 100);
            break;
        case SkillType::MINING:
            bonusText = StringUtils::format("Ore acquisition: +%.0f%%", (bonus - 1.0f) * 100);
            break;
        case SkillType::FISHING:
            bonusText = StringUtils::format("Fishing time: -%.0f%%", (1.0f - bonus) * 100);
            break;
        case SkillType::COOKING:
            bonusText = StringUtils::format("Cooking success rate: +%.0f%%", (bonus - 1.0f) * 100);
            break;
    }

    // 创建标题
    auto titleLabel = Label::createWithSystemFont(skillName + " Level Up!", "Arial", 24);
    titleLabel->setPosition(Vec2(150, 110));
    titleLabel->setColor(Color3B::YELLOW);
    bgLayer->addChild(titleLabel);

    // 创建等级文本
    auto levelLabel = Label::createWithSystemFont("Level " + std::to_string(newLevel), "Arial", 22);
    levelLabel->setPosition(Vec2(150, 70));
    levelLabel->setColor(Color3B::WHITE);
    bgLayer->addChild(levelLabel);

    // 创建加成文本
    auto bonusLabel = Label::createWithSystemFont(bonusText, "Arial", 20);
    bonusLabel->setPosition(Vec2(150, 30));
    bonusLabel->setColor(Color3B::GREEN);
    bgLayer->addChild(bonusLabel);

    // 设置弹窗位置（在玩家头顶上方）
    Vec2 tipPos = Vec2(
        playerPos.x,
        playerPos.y + 100  // 在玩家上方100像素
    );
    bgLayer->setPosition(tipPos - Vec2(150, 75));  // 居中显示

    // 添加到场景
    Director::getInstance()->getRunningScene()->addChild(bgLayer, 1000);

    // 创建入场动画
    bgLayer->setScale(0.1f);
    bgLayer->runAction(ScaleTo::create(0.2f, 1.0f));

    // 添加点击监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [bgLayer](Touch* touch, Event* event) {
        // 创建消失动画
        auto fadeOut = FadeOut::create(0.2f);
        auto remove = RemoveSelf::create();
        bgLayer->runAction(Sequence::create(fadeOut, remove, nullptr));
        return true;  // 消费这个触摸事件
        };

    // 启用点击监听
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, bgLayer);

    // 添加自动消失定时器（如果10秒内没有点击）
    auto autoHide = Sequence::create(
        DelayTime::create(10.0f),
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr
    );
    bgLayer->runAction(autoHide);
}

/*
 * 更新技能显示
 * @param type 技能类型
 * @param level 当前等级
 * @param exp 当前经验值
 * @param expNeeded 升级所需经验值
 * 功能：更新UI上显示的技能等级和经验值
 */
void SkillUI::updateSkillDisplay(SkillType type, int level, int exp, int expNeeded) {
    // 更新等级显示
    if (levelLabels.find(type) != levelLabels.end()) {
        levelLabels[type]->setString("Level " + std::to_string(level));
    }

    // 更新经验值显示
    if (bonusLabels.find(type) != bonusLabels.end()) {
        std::string expText = "EXP: " + std::to_string(exp) + "/" + std::to_string(expNeeded);
        bonusLabels[type]->setString(expText);
    }
}

/*
 * 初始化技能UI
 * @return 初始化是否成功
 * 功能：创建并初始化技能界面的所有UI元素
 */
bool SkillUI::init() {
    if (!Node::init()) {
        return false;
    }

    // 设置更高的Z序
    this->setLocalZOrder(1000);
    // 创建背景图片
    auto background = Sprite::create("LooseSprites/JunimoNoteMobile.png");
    if (background) {
        background->setPosition(Vec2(130, 70));
        background->setScale(0.3f);
        this->addChild(background);
    }

    // 初始化所有技能的UI元素
    const std::vector<SkillType> skills = {
        SkillType::FARMING, SkillType::MINING,
        SkillType::FISHING, SkillType::COOKING
    };

    // 从起始位置开始逐个创建技能显示
    float currentY = START_Y;
    for (const auto& type : skills) {
        // 创建技能名称标签
        auto nameLabel = Label::createWithSystemFont(getSkillName(type), "Arial", 13);
        if (nameLabel) {
            nameLabel->setPosition(Vec2(100, currentY));
            nameLabel->setScale(0.3f);
            nameLabel->setColor(Color3B::BLACK);
            this->addChild(nameLabel);
        }

        // 创建等级标签
        auto levelLabel = Label::createWithSystemFont("Level 1", "Arial", 13);
        if (levelLabel) {
            levelLabel->setPosition(Vec2(130, currentY));
            levelLabel->setScale(0.3f);
            levelLabel->setColor(Color3B::BLACK);
            this->addChild(levelLabel);
            levelLabels[type] = levelLabel;
        }

        // 创建经验值标签
        auto expLabel = Label::createWithSystemFont("EXP: 0/5", "Arial", 13);
        if (expLabel) {
            expLabel->setPosition(Vec2(160, currentY));
            expLabel->setScale(0.3f);
            expLabel->setColor(Color3B::BLACK);
            this->addChild(expLabel);
            bonusLabels[type] = expLabel;
        }

        currentY -= SPACING_Y * 0.3f;  // 更新下一个技能的位置
    }

    return true;
}