#pragma once
#include <map>
#include "cocos2d.h"
#include "SkillUI.h"
#include "SkillType.h"
USING_NS_CC;

class SkillSystem {
public:
    // 获取单例实例
    static SkillSystem* getInstance();

    // 设置UI引用
    void setSkillUI(SkillUI* ui) { skillUI = ui; }

    // 获取技能等级
    int getSkillLevel(SkillType type);

    // 计算经验所需值
    int calculateExpNeeded(int level);


    // 获取技能经验值
    int getSkillExp(SkillType type);

    // 增加经验值
    void gainExp(SkillType type, int amount);

    // 获取技能加成
    float getSkillBonus(SkillType type);

private:

    const int MAX_LEVEL = 10;                 // 最大等级
    const int BASE_EXP_NEEDED = 5;            // 每级所需基础经验

    SkillSystem();                            // 私有构造函数
    static SkillSystem* _instance;            // 单例实例
    SkillUI* skillUI;                         // UI引用

    std::map<SkillType, int> skillLevels;     // 技能等级
    std::map<SkillType, int> skillExp;        // 技能经验

};