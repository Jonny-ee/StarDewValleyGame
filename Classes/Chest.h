#pragma once
#include "cocos2d.h"
#include "ItemSystem.h"
#include "GameScene.h"
#include "Chest.h"

/*
 * ������
 * ���ܣ�ʵ����Ϸ�еı���ϵͳ
 * 1. �ڵ�ͼ����ʾ���侫��
 * 2. ������Ŀ���״̬
 */

class Chest : public cocos2d::Sprite
{
public:
    // ��������ľ�̬����
    static Chest* create();

    // ��ʼ������
    virtual bool init();

    // ��ȡ����״̬
    bool isOpened() const { return opened; }

    // ���ñ���λ��
    void setChestPosition(const cocos2d::Vec2& position);

    // ��ʼ�������¼�
    void initTouchEvents();

    // �������Ƿ��ڿɽ�����Χ��
    bool isPlayerInRange(const cocos2d::Vec2& playerPos) const;

private:
    bool opened;     // ���俪��״̬

    // �������س���
    const std::string CHEST_SPRITE_FILE = "Chest.png";
    const int FRAME_WIDTH = 48;        // ÿ֡���
    const int FRAME_HEIGHT = 48;       // ÿ֡�߶�
    const int TOTAL_FRAMES = 5;       // ��֡��
    const float ANIMATION_SPEED = 0.2f;  // ÿ֡������ʱ����
    const float INTERACTION_RANGE = 100.0f;  // ��ҿ��Կ�������ľ���

    // ��������Ʒ�������
    const std::vector<std::string> possibleItems = {
        "wood", "apple", "corn", "bread",
        "tomato", "fish", "stone"
    };

    const int MIN_ITEMS = 1;    // ����������Ʒ����
    const int MAX_ITEMS = 3;    // ���������Ʒ����

    // ˽�з���
    void openChest();                          // ��������
    void playOpenAnimation();                  // ���ſ�������
    void onOpenAnimationFinished();            // ������ɻص�
    std::string generateRandomItem() const;     // ���������Ʒ
    cocos2d::Animation* createChestAnimation(); // �������䶯��
    void createItemObtainEffect(const std::string& itemId, int index);
};