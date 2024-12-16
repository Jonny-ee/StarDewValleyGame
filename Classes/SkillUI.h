#pragma once
#include "cocos2d.h"
#include "SkillType.h"
#include <map>
#include "Player.h"

class SkillUI : public cocos2d::Node {
public:
    static SkillUI* create();
    virtual bool init() override;

    // 切换显示/隐藏
    void toggleVisibility();

    // 更新技能显示
    void updateSkillDisplay(SkillType type, int level, int exp, int expNeeded);

    // 显示升级提示
    void showLevelUpTip(SkillType type, int newLevel, float bonus);

    // 获取实例（供 GameScene 使用）
    static SkillUI* getInstance() { return _instance; }

private:
    void createUI();
    std::string getSkillName(SkillType type);

    // UI元素
    cocos2d::Sprite* background{ nullptr };  // 背景图片
    std::map<SkillType, cocos2d::Label*> levelLabels;   // 等级标签
    std::map<SkillType, cocos2d::Label*> bonusLabels;   // 加成标签
    std::map<SkillType, cocos2d::Label*> nameLabels;    // 名称标签

    // UI布局常量
    const float START_Y = 85;    // 起始Y坐标
    const float SPACING_Y = 30;   // 垂直间距
    const float NAME_X = 80;      // 名称X坐标
    const float LEVEL_X = 180;    // 等级X坐标
    const float BONUS_X = 280;    // 加成X坐标

    static SkillUI* _instance;  // 单例实例
};