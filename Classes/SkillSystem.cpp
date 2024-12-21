#include "SkillSystem.h"

/*
 * ����ϵͳ
 * ���ܣ�������Ϸ�еļ���ϵͳ�������ȼ�������ֵ�ͼ��ܼӳ�
 */

 // ��ʼ����̬ʵ��ָ��
SkillSystem* SkillSystem::_instance = nullptr;

/*
 * ��ȡ����ϵͳ����ʵ��
 * @return ���ؼ���ϵͳʵ��ָ��
 */
SkillSystem* SkillSystem::getInstance() {
    if (_instance == nullptr) {
        _instance = new SkillSystem();
    }
    return _instance;
}

/*
 * ����ϵͳ���캯��
 * ���ܣ���ʼ�����м��ܵĵȼ��;���ֵ
 */
SkillSystem::SkillSystem() : skillUI(nullptr) {
    // ��ʼ��ÿ�����ܵĵȼ��;���
    for (auto type : { SkillType::FARMING, SkillType::MINING,
                     SkillType::FISHING, SkillType::COOKING }) {
        skillLevels[type] = 1;  // ��ʼ�ȼ�Ϊ1
        skillExp[type] = 0;     // ��ʼ����Ϊ0
    }
}

/*
 * ��ȡָ�����ܵĵȼ�
 * @param type ��������
 * @return ���ؼ��ܵ�ǰ�ȼ�
 */
int SkillSystem::getSkillLevel(SkillType type) {
    return skillLevels[type];
}

/*
 * ��ȡָ�����ܵľ���ֵ
 * @param type ��������
 * @return ���ؼ��ܵ�ǰ����ֵ
 */
int SkillSystem::getSkillExp(SkillType type) {
    return skillExp[type];
}

/*
 * �����������辭��ֵ
 * @param level ��ǰ�ȼ�
 * @return ����������һ������ľ���ֵ
 */
int SkillSystem::calculateExpNeeded(int level) {
    return BASE_EXP_NEEDED * level;
}

/*
 * ��ü��ܾ���ֵ
 * @param type ��������
 * @param amount ��õľ���ֵ����
 * ���ܣ����Ӽ��ܾ���ֵ�����������߼�
 */
void SkillSystem::gainExp(SkillType type, int amount) {
    // �Ѵﵽ��ߵȼ��������Ӿ���
    if (skillLevels[type] >= MAX_LEVEL) {
        return;
    }

    // ���Ӿ���ֵ
    skillExp[type] += amount;

    // ����Ƿ��������
    while (true) {
        int currentLevel = skillLevels[type];
        int expNeeded = calculateExpNeeded(currentLevel);

        // �������ֵ�㹻��δ�ﵽ��ߵȼ�
        if (skillExp[type] >= expNeeded && currentLevel < MAX_LEVEL) {
            // �������۳�����ֵ
            skillLevels[type]++;
            skillExp[type] -= expNeeded;

            // ����UI��ʾ
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

    // ����UI��ʾ
    if (skillUI) {
        skillUI->updateSkillDisplay(type, skillLevels[type],
            skillExp[type], calculateExpNeeded(skillLevels[type]));
    }
}

/*
 * ��ȡ���ܼӳ�
 * @param type ��������
 * @return ���ؼ��ܼӳ�ϵ��
 * ���ܣ����ݼ��ܵȼ����㲻ͬ���ܵļӳ�Ч��
 */
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
            if (bonus < 0.5f) bonus = 0.5f; // �����ٵ�50%ʱ��
            break;

        case SkillType::COOKING:
            // ÿ��һ����ʳ�������������
            bonus = pow(2.0f, level - 1);
            break;
    }
    return bonus;
}