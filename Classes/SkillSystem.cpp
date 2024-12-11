#include "SkillSystem.h"

// 初始化静态成员变量
SkillSystem* SkillSystem::_instance = nullptr;

// 获取单例实例
SkillSystem* SkillSystem::getInstance() {
    if (_instance == nullptr) {
        _instance = new SkillSystem();
    }
    return _instance;
}

// 构造函数，初始化技能等级和经验
SkillSystem::SkillSystem() : skillUI(nullptr) {
    // 初始化每个技能的等级和经验
    for (auto type : { SkillType::FARMING, SkillType::MINING,
                     SkillType::FISHING, SkillType::COOKING }) {
        skillLevels[type] = 1;  // 初始等级为1
        skillExp[type] = 0;     // 初始经验为0
    }
}

// 获取指定技能的等级
int SkillSystem::getSkillLevel(SkillType type) {
    return skillLevels[type];
}

// 获取指定技能的经验值
int SkillSystem::getSkillExp(SkillType type) {
    return skillExp[type];
}

// 计算升级所需的经验值
int SkillSystem::calculateExpNeeded(int level) {
    // 每级所需经验值
    return BASE_EXP_NEEDED * level;
}

// 增加指定技能的经验值
void SkillSystem::gainExp(SkillType type, int amount) {
    // 如果已经达到最大等级，则不再增加经验
    if (skillLevels[type] >= MAX_LEVEL) {
        return;
    }

    // 增加经验值
    skillExp[type] += amount;

    // 检查是否可以升级
    while (true) {
        int currentLevel = skillLevels[type];
        int expNeeded = calculateExpNeeded(currentLevel);

        // 如果经验值足够升级，并且未达到最大等级
        if (skillExp[type] >= expNeeded && currentLevel < MAX_LEVEL) {
            // 提升技能等级
            skillLevels[type]++;
            skillExp[type] -= expNeeded;

            // 如果 UI 存在，更新 UI 显示
            if (skillUI) {
                float bonus = getSkillBonus(type);
                // 显示升级提示
                skillUI->showLevelUpTip(type, skillLevels[type], bonus);
                // 更新技能显示，包括等级、经验和所需经验
                skillUI->updateSkillDisplay(type, skillLevels[type],
                    skillExp[type], calculateExpNeeded(skillLevels[type]));
            }
        }
        else {
            break;
        }
    }

    // 无论是否升级，更新 UI 显示
    if (skillUI) {
        skillUI->updateSkillDisplay(type, skillLevels[type],
            skillExp[type], calculateExpNeeded(skillLevels[type]));
    }
}

// 获取技能加成
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
            if (bonus < 0.5f) bonus = 0.5f; // 最低减少到50%时间
            break;
        case SkillType::COOKING:
            // 每级增加20%烹饪成功率
            bonus += (level - 1) * 0.2f;
            break;
    }
    return bonus;
}