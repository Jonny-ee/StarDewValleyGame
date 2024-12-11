#pragma once
#include <map>
#include "cocos2d.h"
#include "SkillUI.h"
#include "SkillType.h"
USING_NS_CC;

class SkillSystem {
public:
    // ��ȡ����ʵ��
    static SkillSystem* getInstance();

    // ��ȡ���ܵȼ�
    int getSkillLevel(SkillType type);

    // ��ȡ���ܾ���ֵ
    int getSkillExp(SkillType type);

    // ���Ӿ���ֵ
    void gainExp(SkillType type, int amount);

    // ��ȡ���ܼӳ�
    float getSkillBonus(SkillType type);

    // ����UI����
    void setSkillUI(SkillUI* ui) { skillUI = ui; }

    // ���㾭������ֵ
    int calculateExpNeeded(int level);

private:
    SkillSystem();  // ˽�й��캯��
    static SkillSystem* _instance;  // ����ʵ��

    std::map<SkillType, int> skillLevels;  // ���ܵȼ�
    std::map<SkillType, int> skillExp;     // ���ܾ���

    const int MAX_LEVEL = 10;              // ���ȼ�
    const int BASE_EXP_NEEDED = 10;        // ÿ�������������


    // UI����
    SkillUI* skillUI;
};