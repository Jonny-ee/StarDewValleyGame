#pragma once
#include "cocos2d.h"

#include <string>
#include <map>
#include <vector>

/* NPC类 */
class NPC :public cocos2d::Sprite//继承自精灵类
{
public:
    //心情
    enum class Mood
    {
        HAPPY,
        NEUTRAL,
        ANGRY,
        SAD,
        LOVE
    };
    //日程状态
    enum class Schedule
    {
        SLEEPING,
        WORKING,
        RELAXING,
        SOCIALIZING,
        EATING
    };
    //季节
    enum class Season
    {
        SPRING,
        SUMMER,
        AUTUMN,
        WINTER
    };
    //创建NPC
    static NPC* create(const std::string& npcImagePath);
    virtual bool init(const std::string& npcImagePath);
    //设置基本信息
    void setName(const std::string& name) { npcName = name; }
    void setOccupation(const std::string& job) { occupation = job; }
    void setBirthday(Season season, int day) noexcept;
    //好感度系统
    void addHeartPoints(int points) noexcept;
    int gerHeartLevel() const noexcept { return heartLevel; }
    bool canReceiveGiftToday() const noexcept { return canReceiveGift; }
    //礼物系统
    void receiveGift(const std::string& item);
    void setFavoriteGift(const std::vector<std::string>& gifts) { favoriteGifts = gifts; }
    void setDislikedGift(const std::vector<std::string>& gifts) { dislikedGifts = gifts; }
    //对话系统
    void addDialogue(int heartLevel, const std::string& dialogue);
    std::string getRandomDialogue() const;
    //日程系统
    void setSchedule(Season season, int weekDay, const std::map<int, std::string>& daySchedule);
    void updateSchedule(float dt);
    Schedule getCurrentSchedule() const noexcept { return currentSchedule; }
    //互动系统
    void interact();
    void startConversation();
    void showGiftMenu();
    //事件系统
    void triggerEvent(const std::string& eventId);
    bool hasCompletedEvent(const std::string& eventId) const;
    //动画相关
    void updateMood(Mood newMood);
protected:
    //基本信息
    std::string npcName;//姓名
    std::string occupation;//职业
    int birthdaySeason;//生日季节
    int brithdayDay;//生日天数
    //好感度
    int heartPoint;
    int heartLevel;
    bool canReceiveGift;
    //心情
    Mood currentMood;
    //礼物系统
    std::vector<std::string> favoriteGifts;
    std::vector<std::string> dislikedGifts;
    std::vector<std::string> receivedGifts;
    //对话系统
    std::map<int, std::vector<std::string>> dialogues;//按好感度等级存储对话
    std::map<std::string, std::string> eventDialogues;//特殊事件对话
    //日程系统
    Schedule currentSchedule;
    std::map<Season, std::map<int, std::map<int, std::string>>> schedules;//季节->星期->时间->位置
    cocos2d::Vec2 currentDestination;
    float moveSpeed;
    //事件系统
    std::vector<std::string> completedEvents;
    //动画相关
    cocos2d::Action* walkAnimation;
    cocos2d::Action* currentAnimation;
    //私有方法
    void initializeAnimations();//初始化动画
    void calculateHeartLevel() noexcept;//计算好感度等级
    void updateDailySchedule(float dt);
    std::string getScheduleLocation(Season season, int weekDay, int hour) const;
    void loadDefaultDialogues();
    void updateRelationshipStatus();
};