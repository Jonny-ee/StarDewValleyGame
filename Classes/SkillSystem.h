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

    // ����UI����
    void setSkillUI(SkillUI* ui) { skillUI = ui; }

    // ��ȡ���ܵȼ�
    int getSkillLevel(SkillType type);

    // ���㾭������ֵ
    int calculateExpNeeded(int level);


    // ��ȡ���ܾ���ֵ
    int getSkillExp(SkillType type);

    // ���Ӿ���ֵ
    void gainExp(SkillType type, int amount);

    // ��ȡ���ܼӳ�
    float getSkillBonus(SkillType type);

private:

    const int MAX_LEVEL = 10;                 // ���ȼ�
    const int BASE_EXP_NEEDED = 5;            // ÿ�������������

    SkillSystem();                            // ˽�й��캯��
    static SkillSystem* _instance;            // ����ʵ��
    SkillUI* skillUI;                         // UI����

    std::map<SkillType, int> skillLevels;     // ���ܵȼ�
    std::map<SkillType, int> skillExp;        // ���ܾ���

};