#include "SkillSystem.h"

/*
 * 技能系统
 * 功能：管理游戏中的技能系统，包括等级、经验值和技能加成
 */

 // 初始化静态实例指针
SkillSystem* SkillSystem::_instance = nullptr;

/*
 * 获取技能系统单例实例
 * @return 返回技能系统实例指针
 */
SkillSystem* SkillSystem::getInstance() {
    if (_instance == nullptr) {
        _instance = new SkillSystem();
    }
    return _instance;
}

/*
 * 技能系统构造函数
 * 功能：初始化所有技能的等级和经验值
 */
SkillSystem::SkillSystem() : skillUI(nullptr) {
    // 初始化每个技能的等级和经验
    for (auto type : { SkillType::FARMING, SkillType::MINING,
                     SkillType::FISHING, SkillType::COOKING }) {
        skillLevels[type] = 1;  // 初始等级为1
        skillExp[type] = 0;     // 初始经验为0
    }
}

/*
 * 获取指定技能的等级
 * @param type 技能类型
 * @return 返回技能当前等级
 */
int SkillSystem::getSkillLevel(SkillType type) {
    return skillLevels[type];
}

/*
 * 获取指定技能的经验值
 * @param type 技能类型
 * @return 返回技能当前经验值
 */
int SkillSystem::getSkillExp(SkillType type) {
    return skillExp[type];
}

/*
 * 计算升级所需经验值
 * @param level 当前等级
 * @return 返回升到下一级所需的经验值
 */
int SkillSystem::calculateExpNeeded(int level) {
    return BASE_EXP_NEEDED * level;
}

/*
 * 获得技能经验值
 * @param type 技能类型
 * @param amount 获得的经验值数量
 * 功能：增加技能经验值并处理升级逻辑
 */
void SkillSystem::gainExp(SkillType type, int amount) {
    // 已达到最高等级则不再增加经验
    if (skillLevels[type] >= MAX_LEVEL) {
        return;
    }

    // 增加经验值
    skillExp[type] += amount;

    // 检查是否可以升级
    while (true) {
        int currentLevel = skillLevels[type];
        int expNeeded = calculateExpNeeded(currentLevel);

        // 如果经验值足够且未达到最高等级
        if (skillExp[type] >= expNeeded && currentLevel < MAX_LEVEL) {
            // 升级并扣除经验值
            skillLevels[type]++;
            skillExp[type] -= expNeeded;

            // 更新UI显示
            if (skillUI) {
                float bonus = getSkillBonus(type);
                skillUI->showLevelUpTip(type, skillLevels[type], bonus);
                skillUI->updateSkillDisplay(type, skillLevels[type],
                    skillExp[type], calculateExpNeeded(skillLevels[type]));
            }
        }
        else {
            break;
        }
    }

    // 更新UI显示
    if (skillUI) {
        skillUI->updateSkillDisplay(type, skillLevels[type],
            skillExp[type], calculateExpNeeded(skillLevels[type]));
    }
}

/*
 * 获取技能加成
 * @param type 技能类型
 * @return 返回技能加成系数
 * 功能：根据技能等级计算不同技能的加成效果
 */
float SkillSystem::getSkillBonus(SkillType type) {
    int level = skillLevels[type];
    float bonus = 1.0f;

    switch (type) {
        case SkillType::FARMING:
            // 每级增加10%收获量
            bonus += (level - 1) * 0.1f;
            break;

        case SkillType::MINING:
            // 每级增加15%矿石获取量
            bonus += (level - 1) * 0.15f;
            break;

        case SkillType::FISHING:
            // 每级减少5%钓鱼时间
            bonus -= (level - 1) * 0.05f;
            if (bonus < 0.5f) bonus = 0.5f; // 最多减少到50%时间
            break;

        case SkillType::COOKING:
            // 每升一级，食物产出增加两倍
            bonus = pow(2.0f, level - 1);
            break;
    }
    return bonus;
}
