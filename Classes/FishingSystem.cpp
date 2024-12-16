#include "FishingSystem.h"
#include "ItemSystem.h"

USING_NS_CC;

FishingSystem* FishingSystem::_instance = nullptr;

/*
 * ��ȡ����ϵͳ����ʵ��
 * @return ����ϵͳʵ��ָ��
 */
FishingSystem* FishingSystem::getInstance()
{
    if (!_instance)     // ���ʵ�������ڣ�������ʵ��
    {
        _instance = new FishingSystem();
    }
    return _instance;
}

/*
 * ��ʼ����������
 * @param gameMap ��ǰ��Ϸ��ͼָ��
 */
void FishingSystem::initFishingAreas(GameMap* gameMap)
{
    fishingAreas.clear(); // ������еĵ�������

    // ��鵱ǰ��ͼ�Ƿ��� Farm
    if (gameMap->getMapName() == "Farm")
    {
        // ��ũ����ͼ��ӵ������򣬲���Ϊ��x���꣬y���꣬��ȣ��߶�
        fishingAreas.push_back(cocos2d::Rect(820, 510, 240, 280));
    }
}

/*
 * ���λ���Ƿ��ڵ���������
 * @param position Ҫ����λ��
 * @return �Ƿ��ڵ���������
 */
bool FishingSystem::isInFishingArea(const Vec2& position)
{
    if (fishingAreas.empty())   // ���û�е�������ֱ�ӷ���false
    {
        return false;
    }

    for (const auto& area : fishingAreas)
    {
        if (area.containsPoint(position))   // ���λ���Ƿ�����һ����������
        {
            return true;
        }
    }
    return false;
}

/*
 * �������Ƿ�װ�������
 * @param player ��Ҷ���ָ��
 * @return �Ƿ�װ�����
 */
bool FishingSystem::hasEquippedFishingRod(Player* player)
{
    return player->getCurrentTool() == Player::ToolType::ROD;   // ��鵱ǰ�����Ƿ�Ϊ���
}

/*
 * �������Ƿ���Խ��е���
 * @param playerPos ���λ��
 * @param player ��Ҷ���ָ��
 * @return �Ƿ���Ե���
 */
bool FishingSystem::canFish(const Vec2& playerPos, Player* player)
{
    if (!isInFishingArea(playerPos))    // ����Ƿ��ڵ���������
    {
        if (isFishing)  // ������ڵ��㵫�뿪����������״̬
        {
            resetFishingState();
        }

        hideTip();
        return false;
    }

    if (showingResult)  // ���������ʾ���������ʾ������ʾ
    {
        return false;
    }

    if (isFishing)  // ����Ѿ��ڵ�����
    {
        return false;
    }

    if (!hasEquippedFishingRod(player)) // ����Ƿ�װ�����
    {
        showTip("Need fishing rod");
        return false;
    }

    showTip("Click to start fishing");  // ��ʾ��ʼ������ʾ
    return true;
}

/*
 * ��ʼ����
 * ��ʼ������״̬����ʾ��Ӧ��ʾ
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

        // ʹ��ʵ�ʵ���ʱ�䣨���Ǽ��ܼӳɣ�
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
 * ��ɵ���
 * �����������������Ʒ��ȡ����ʾ��ʾ
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
            if (randomNum < 70)     // 70%���ʵ�������
            {
                randomNum = rand() % 100;
                if (randomNum < 80)     // 80%���ʵ�����ͨ��
                {
                    ItemSystem::getInstance()->addItem("fish", 1);

                    // ���1�㾭�鲢��ʾ��ʾ
                    int prevLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);
                    SkillSystem::getInstance()->gainExp(SkillType::FISHING, 1);
                    int currentLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);

                    std::string message = "You caught a fish!\nFishing EXP +1";

                    // ��������ˣ����������ʾ
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
                else    // ����������֮��
                {
                    ItemSystem::getInstance()->addItem("mermaid's KISS(*)", 1);

                    // ���5�㾭�鲢��ʾ��ʾ
                    int prevLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);
                    SkillSystem::getInstance()->gainExp(SkillType::FISHING, 5);
                    int currentLevel = SkillSystem::getInstance()->getSkillLevel(SkillType::FISHING);

                    std::string message = "You found a mermaid!\nShe left you a kiss and disappeared...\nFishing EXP +5";

                    // ��������ˣ���ʾ
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
            else    // ��30%�����ܵ�
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
 * ��ʾ��ʾ�ı�
 * @param text Ҫ��ʾ���ı�
 * @param duration ��ʾ����ʱ�䣨0��ʾ������ʾ��
 */
void FishingSystem::showTip(const std::string& text, float duration)
{
    if (!tipLabel)  // �����ʾ��ǩ�����ڣ������µ�
    {
        tipLabel = Label::createWithSystemFont(text, "Arial", 24);
        if (tipLabel)
        {
            tipLabel->setPosition(Vec2(0, 0));  // ��ʼλ����Ϊԭ�㣬������update�и���
            Director::getInstance()->getRunningScene()->addChild(tipLabel, 10);
        }
    }

    tipLabel->setString(text);  // ������ʾ�ı�
    tipLabel->setVisible(true); // ��ʾ��ʾ

    if (duration > 0)   // ��������˳���ʱ�䣬�����Զ���������
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
 * ������ʾ�ı�
 */
void FishingSystem::hideTip()
{
    if (tipLabel)   // �����ʾ��ǩ���ڣ�����Ϊ���ɼ�
    {
        tipLabel->setVisible(false);
    }
}