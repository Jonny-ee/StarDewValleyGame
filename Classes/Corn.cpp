#include "Corn.h"

/*
 * �������ﹹ�캯��
 * ��ʼ�����׵Ļ�������
 */
Corn::Corn() : Crop("Corn", 8, 1, "Plants.png") {}

/*
 * ������������ʵ��
 * @param position �����������������λ��
 * @return �����ɹ���������ʵ��ָ�룬ʧ�ܷ���nullptr
 */
Corn* Corn::create(const Vec2& position)
{
    Corn* corn = new (std::nothrow) Corn();
    if (corn && corn->init())
    {
        corn->autorelease();
        corn->setPosition(position);
        CCLOG("Created corn at position: (%.1f, %.1f)", position.x, position.y);
        return corn;
    }
    CC_SAFE_DELETE(corn);
    return nullptr;
}

/*
 * ��ʼ����������
 * ���ó�ʼ�������ʾ����
 * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
 */
bool Corn::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // ֱ����������ͳ�ʼ����
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 0, 16, 16));
    this->setScale(2.0f);

    CCLOG("Corn initialized with texture");
    return true;
}

/*
 * �������������׶�
 * @param stage Ŀ�������׶�(0-4: ���ӡ����硢�����ڡ������ڡ��ջ���)
 */
void Corn::updateGrowthStage(int stage)
{
    if (stage >= 0 && stage < TOTAL_GROWTH_STAGES)
    {
        this->setTextureRect(Rect(stage * 16, 0, 16, 16));
        CCLOG("Updated corn growth stage to: %d", stage);
    }
}