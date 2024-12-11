#include "NPC.h"

USING_NS_CC;

//创建NPC实例
NPC* NPC::create(const std::string& npcImagePath)
{
    NPC* npc = new (std::nothrow) NPC();
    if (npc && npc->init(npcImagePath)) {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}
//初始化NPC
bool NPC::init(const std::string& npcImagePath)
{
    if (!Sprite::initWithFile(npcImagePath))
    {
        return false;
    }
    //初始化基本信息
    moveSpeed = 50.0f;//移速
    heartLevel = 0;//好感度
    heartPoint = 0;//好感等级
    canReceiveGift = true;//默认可以接收礼物
    currentMood = Mood::NEUTRAL;//心情
    currentSchedule = Schedule::WORKING;//日程
    //设置精灵属性
    this->setAnchorPoint(Vec2(0.5f, 0.5f));//设置锚点
    this->setScale(2.0f);//设置缩放
    //初始化动画
    initializeAnimations();
    //启动调度器 
    this->schedule(CC_SCHEDULE_SELECTOR(NPC::updateDailySchedule), 1.0f);
    return true;
}
//设置生日
void NPC::setBirthday(Season season, int day) noexcept
{
    birthdaySeason = static_cast<int>(season);
    brithdayDay = day;
}
//添加好感度
void NPC::addHeartPoints(int points) noexcept
{
    heartPoint += points;
    calculateHeartLevel();
}
//计算好感度等级
void NPC::calculateHeartLevel() noexcept
{
    //每1000点升一级
    heartLevel = heartPoint / 1000;
    if (heartLevel > 10) heartLevel = 10;//最高10级
}
//接收礼物
void NPC::receiveGift(const std::string& item)
{
    if (!canReceiveGift) return;

    int points = 0;
    if (std::find(favoriteGifts.begin(), favoriteGifts.end(), item) != favoriteGifts.end())
    {
        points = 100;//喜欢的礼物
        currentMood = Mood::HAPPY;
    }
    else if (std::find(dislikedGifts.begin(), dislikedGifts.end(), item) != dislikedGifts.end())
    {
        points = -50;//讨厌的礼物
        currentMood = Mood::ANGRY;
    }
    else
    {
        points = 20;//普通礼物
        currentMood = Mood::NEUTRAL;
    }
    addHeartPoints(points);
    receivedGifts.push_back(item);
    canReceiveGift = false;//一天只能送一次礼物，这部分要和后面时间联系起来***********************
}
//添加对话
void NPC::addDialogue(int heartLevel, const std::string& dialogue)
{
    dialogues[heartLevel].push_back(dialogue);
}
//获取随机对话
std::string NPC::getRandomDialogue() const
{
    //获取当前好感度等级的对话列表
    auto it = dialogues.find(heartLevel);
    if (it != dialogues.end() && !it->second.empty())
    {
        int index = rand() % it->second.size();
        return it->second[index];
    }
    return "..."; //默认对话
}
//设置日程
void NPC::setSchedule(Season season, int weekDay, const std::map<int, std::string>& daySchedule)
{
    schedules[season][weekDay] = daySchedule;
}
//更新日程,需要时间***********************************************************************
void NPC::updateSchedule(float dt)
{
    // 获取当前游戏时间和日期（这里需要实现游戏时间系统）
    // auto gameTime = GameTime::getInstance();
    // int currentHour = gameTime->getHour();
    // Season currentSeason = gameTime->getSeason();
    // int currentWeekDay = gameTime->getWeekDay();

    //根据时间更新NPC位置和状态
    updateDailySchedule(dt);
}
//更新日常行程
void NPC::updateDailySchedule(float dt)
{
    //根据当前时间获取目标位置
    std::string locationName = getScheduleLocation(Season::SPRING, 1, 8); // 示例：春季周一8点
    // 移动到目标位置（这里需要实现位置转换系统）
    // Vec2 targetPos = LocationManager::getInstance()->getPosition(locationName);
    // moveToLocation(targetPos);
}
//初始化动画
void NPC::initializeAnimations()
{
    //创建行走动画
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.2f);

    //假设精灵表有4帧动画
    float frameWidth = this->getContentSize().width / 4;
    float frameHeight = this->getContentSize().height / 4;

    //  for (int i = 0; i < 4; i++)
    //  {
          // auto frame = cocos2d::SpriteFrame::create(
              //this->getTexture(),
               //cocos2d::Rect(i * frameWidth, 0, frameWidth, frameHeight)
          // );
          // animation->addSpriteFrame(frame);
     // }

    walkAnimation = RepeatForever::create(Animate::create(animation));
    walkAnimation->retain();
}

//互动
void NPC::interact()
{
    std::string dialogue = getRandomDialogue();
    // 显示对话框（需要实现对话框系统）
    // DialogueManager::getInstance()->showDialogue(dialogue);
}
//开始对话
void NPC::startConversation()
{
    // 实现对话系统
    interact();
}
//显示礼物菜单
void NPC::showGiftMenu()
{
    // 实现礼物菜单UI
    // GiftMenuUI::create()->show();
}
//触发事件
void NPC::triggerEvent(const std::string& eventId)
{
    // 处理特殊事件
    std::string dialogue = eventDialogues[eventId];
    // DialogueManager::getInstance()->showDialogue(dialogue);
    completedEvents.push_back(eventId);
}
//检查事件是否完成
bool NPC::hasCompletedEvent(const std::string& eventId) const
{
    return std::find(completedEvents.begin(), completedEvents.end(), eventId) != completedEvents.end();
}
//更新心情
void NPC::updateMood(Mood newMood)
{
    currentMood = newMood;
    // 可以在这里添加心情变化的视觉效果
}
//获取指定时间的位置
std::string NPC::getScheduleLocation(Season season, int weekDay, int hour) const
{
    auto seasonIt = schedules.find(season);
    if (seasonIt != schedules.end())
    {
        auto weekIt = seasonIt->second.find(weekDay);
        if (weekIt != seasonIt->second.end())
        {
            auto hourIt = weekIt->second.find(hour);
            if (hourIt != weekIt->second.end())
            {
                return hourIt->second;
            }
        }
    }
    return "home"; // 默认位置
}
//加载默认对话
void NPC::loadDefaultDialogues()
{
    // 添加一些默认对话
   /* addDialogue(0, "你好啊！");
    addDialogue(0, "今天天气真不错。");
    addDialogue(5, "很高兴见到你！");
    addDialogue(10, "你是我最好的朋友！");*/
}
//更新关系状态
void NPC::updateRelationshipStatus()
{
    // 根据好感度更新关系状态
    // 可以添加更多关系相关的逻辑
}







