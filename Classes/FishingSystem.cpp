#include "FishingSystem.h"
#include "ItemSystem.h"

USING_NS_CC;

FishingSystem* FishingSystem::_instance = nullptr;

/*
 * 获取钓鱼系统单例实例
 * @return 钓鱼系统实例指针
 */
FishingSystem* FishingSystem::getInstance()
{
    if (!_instance)     // 如果实例不存在，创建新实例
    {
        _instance = new FishingSystem();
    }
    return _instance;
}

/*
 * 初始化钓鱼区域
 * @param gameMap 当前游戏地图指针
 */
void FishingSystem::initFishingAreas(GameMap* gameMap)
{
    fishingAreas.clear(); // 清空现有的钓鱼区域

    // 检查当前地图是否是 Farm
    if (gameMap->getMapName() == "Farm")
    {
        // 在农场地图添加钓鱼区域，参数为：x坐标，y坐标，宽度，高度
        fishingAreas.push_back(cocos2d::Rect(820, 510, 240, 280));
    }
}

/*
 * 检查位置是否在钓鱼区域内
 * @param position 要检查的位置
 * @return 是否在钓鱼区域内
 */
bool FishingSystem::isInFishingArea(const Vec2& position)
{
    if (fishingAreas.empty())   // 如果没有钓鱼区域，直接返回false
    {
        return false;
    }

    for (const auto& area : fishingAreas)
    {
        if (area.containsPoint(position))   // 检查位置是否在任一钓鱼区域内
        {
            return true;
        }
    }
    return false;
}

/*
 * 检查玩家是否装备了鱼竿
 * @param player 玩家对象指针
 * @return 是否装备鱼竿
 */
bool FishingSystem::hasEquippedFishingRod(Player* player)
{
    return player->getCurrentTool() == Player::ToolType::ROD;   // 检查当前工具是否为鱼竿
}

/*
 * 检查玩家是否可以进行钓鱼
 * @param playerPos 玩家位置
 * @param player 玩家对象指针
 * @return 是否可以钓鱼
 */
bool FishingSystem::canFish(const Vec2& playerPos, Player* player)
{
    if (!isInFishingArea(playerPos))    // 检查是否在钓鱼区域内
    {
        if (isFishing)  // 如果正在钓鱼但离开了区域，重置状态
        {
            resetFishingState();
        }

        hideTip();
        return false;
    }

    if (showingResult)  // 如果正在显示结果，不显示其他提示
    {
        return false;
    }

    if (isFishing)  // 如果已经在钓鱼中
    {
        return false;
    }

    if (!hasEquippedFishingRod(player)) // 检查是否装备鱼竿
    {
        showTip("Need fishing rod");
        return false;
    }

    showTip("Click to start fishing");  // 显示开始钓鱼提示
    return true;
}

/*
 * 开始钓鱼
 * 初始化钓鱼状态并显示相应提示
 */
void FishingSystem::startFishing()
{
    if (!isFishing)
    {
        isFishing = true;
        fishingStartTime = std::chrono::steady_clock::now();
        showTip("Fishing...Do not leave!");

        if (tipLabel)
        {
            tipLabel->stopAllActions();
        }

        // 使用实际钓鱼时间（考虑技能加成）
        int actualDuration = getActualFishingDuration();

        auto sequence = Sequence::create(
            DelayTime::create(actualDuration),
            CallFunc::create([this]() {
                if (isFishing)
                {
                    showTip("Fish on the hook! Click to catch!");
                }
                }),
            nullptr
        );
        if (tipLabel)
        {
            tipLabel->runAction(sequence);
        }
    }
}

/*
 * 完成钓鱼
 * 处理钓鱼结果，包括物品获取和提示显示
 */
void FishingSystem::finishFishing()
{
    if (isFishing)
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - fishingStartTime).count();

        if (elapsedTime >= getActualFishingDuration())
        {
            showingResult = true;
            int randomNum = rand() % 100;
            if (randomNum < 70)     // 70%概率钓到东西
            {
                randomNum = rand() % 100;
                if (randomNum < 80)     // 80%概率钓到普通鱼
                {
                    ItemSystem::getInstance()->addItem("fish", 1);

                    // 获得1点经验并显示提示
                    int prevLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);
                    SkillSystem::getInstance()->gainExp(SkillType::FISHING, 1);
                    int currentLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);

                    std::string message = "You caught a fish!\nFishing EXP +1";

                    // 如果升级了，添加升级提示
                    if (currentLevel > prevLevel)
                    {
                        float timeReduction = (1.0f - SkillSystem::getInstance()->getSkillBonus(SkillType::FISHING)) * 100;
                        message += "\nFishing Level Up! (" + std::to_string(prevLevel) + "->" + std::to_string(currentLevel) + ")\n";
                        message += "Fishing time reduced by " + std::to_string((int)timeReduction) + "%";
                    }

                    showTip(message, 2.0f);

                    auto delay = DelayTime::create(2.0f);
                    auto func = CallFunc::create([this]() {
                        showingResult = false;
                        });
                    if (tipLabel)
                    {
                        tipLabel->runAction(Sequence::create(delay, func, nullptr));
                    }
                }
                else    // 钓到美人鱼之吻
                {
                    ItemSystem::getInstance()->addItem("mermaid's KISS(*)", 1);

                    // 获得5点经验并显示提示
                    int prevLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);
                    SkillSystem::getInstance()->gainExp(SkillType::FISHING, 5);
                    int currentLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);

                    std::string message = "You found a mermaid!\nShe left you a kiss and disappeared...\nFishing EXP +5";

                    // 如果升级了，提示
                    if (currentLevel > prevLevel)
                    {
                        float timeReduction = (1.0f - SkillSystem::getInstance()->getSkillBonus(SkillType::FISHING)) * 100;
                        message += "\nFishing Level Up! (" + std::to_string(prevLevel) + "->" + std::to_string(currentLevel) + ")\n";
                        message += "Fishing time reduced by " + std::to_string((int)timeReduction) + "%";
                    }

                    showTip(message, 3.0f);

                    auto delay = DelayTime::create(3.0f);
                    auto func = CallFunc::create([this]() {
                        showingResult = false;
                        });
                    if (tipLabel)
                    {
                        tipLabel->runAction(Sequence::create(delay, func, nullptr));
                    }
                }
            }
            else    // 鱼30%概率跑掉
            {
                showTip("The fish got away!", 2.0f);

                auto delay = DelayTime::create(2.0f);
                auto func = CallFunc::create([this]() {
                    showingResult = false;
                    });
                if (tipLabel)
                {
                    tipLabel->runAction(Sequence::create(delay, func, nullptr));
                }
            }
        }
        isFishing = false;
    }
}

/*
 * 显示提示文本
 * @param text 要显示的文本
 * @param duration 显示持续时间（0表示持续显示）
 */
void FishingSystem::showTip(const std::string& text, float duration)
{
    if (!tipLabel)  // 如果提示标签不存在，创建新的
    {
        tipLabel = Label::createWithSystemFont(text, "Arial", 24);
        if (tipLabel)
        {
            tipLabel->setPosition(Vec2(0, 0));  // 初始位置设为原点，后续在update中更新
            Director::getInstance()->getRunningScene()->addChild(tipLabel, 10);
        }
    }

    tipLabel->setString(text);  // 设置提示文本
    tipLabel->setVisible(true); // 显示提示

    if (duration > 0)   // 如果设置了持续时间，创建自动隐藏序列
    {
        auto sequence = Sequence::create(
            DelayTime::create(duration),
            CallFunc::create([this]() {
                hideTip();
                }),
            nullptr
        );
        tipLabel->runAction(sequence);
    }
}

/*
 * 隐藏提示文本
 */
void FishingSystem::hideTip()
{
    if (tipLabel)   // 如果提示标签存在，设置为不可见
    {
        tipLabel->setVisible(false);
    }
}