#include "SkillSystem.h"

// ��ʼ����̬��Ա����
SkillSystem* SkillSystem::_instance = nullptr;

// ��ȡ����ʵ��
SkillSystem* SkillSystem::getInstance() {
    if (_instance == nullptr) {
        _instance = new SkillSystem();
    }
    return _instance;
}

// ���캯������ʼ�����ܵȼ��;���
SkillSystem::SkillSystem() : skillUI(nullptr) {
    // ��ʼ��ÿ�����ܵĵȼ��;���
    for (auto type : { SkillType::FARMING, SkillType::MINING,
                     SkillType::FISHING, SkillType::COOKING }) {
        skillLevels[type] = 1;  // ��ʼ�ȼ�Ϊ1
        skillExp[type] = 0;     // ��ʼ����Ϊ0
    }
}

// ��ȡָ�����ܵĵȼ�
int SkillSystem::getSkillLevel(SkillType type) {
    return skillLevels[type];
}

// ��ȡָ�����ܵľ���ֵ
int SkillSystem::getSkillExp(SkillType type) {
    return skillExp[type];
}

// ������������ľ���ֵ
int SkillSystem::calculateExpNeeded(int level) {
    // ÿ�����辭��ֵ
    return BASE_EXP_NEEDED * level;
}

// ����ָ�����ܵľ���ֵ
void SkillSystem::gainExp(SkillType type, int amount) {
    // ����Ѿ��ﵽ���ȼ����������Ӿ���
    if (skillLevels[type] >= MAX_LEVEL) {
        return;
    }

    // ���Ӿ���ֵ
    skillExp[type] += amount;

    // ����Ƿ��������
    while (true) {
        int currentLevel = skillLevels[type];
        int expNeeded = calculateExpNeeded(currentLevel);

        // �������ֵ�㹻����������δ�ﵽ���ȼ�
        if (skillExp[type] >= expNeeded && currentLevel < MAX_LEVEL) {
            // �������ܵȼ�
            skillLevels[type]++;
            skillExp[type] -= expNeeded;

            // ��� UI ���ڣ����� UI ��ʾ
            if (skillUI) {
                float bonus = getSkillBonus(type);
                // ��ʾ������ʾ
                skillUI->showLevelUpTip(type, skillLevels[type], bonus);
                // ���¼�����ʾ�������ȼ�����������辭��
                skillUI->updateSkillDisplay(type, skillLevels[type],
                    skillExp[type], calculateExpNeeded(skillLevels[type]));
            }
        }
        else {
            break;
        }
    }

    // �����Ƿ����������� UI ��ʾ
    if (skillUI) {
        skillUI->updateSkillDisplay(type, skillLevels[type],
            skillExp[type], calculateExpNeeded(skillLevels[type]));
    }
}

// ��ȡ���ܼӳ�
float SkillSystem::getSkillBonus(SkillType type) {
    int level = skillLevels[type];
    float bonus = 1.0f;

    switch (type) {
        case SkillType::FARMING:
            // ÿ������10%�ջ���
            bonus += (level - 1) * 0.1f;
            break;

        case SkillType::MINING:
            // ÿ������15%��ʯ��ȡ��
            bonus += (level - 1) * 0.15f;
            break;

        case SkillType::FISHING:
            // ÿ������5%����ʱ��
            bonus -= (level - 1) * 0.05f;
            if (bonus < 0.5f) bonus = 0.5f; // ��ͼ��ٵ�50%ʱ��
            break;
        case SkillType::COOKING:
            // ÿ������20%��⿳ɹ���
            bonus += (level - 1) * 0.2f;
            break;
    }
    return bonus;
}