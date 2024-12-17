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

    // ���ó�ʼ��������״̬����һ�еڶ������ӣ�
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 0, 16, 16));  // ����״̬
    this->setScale(2.0f);

    CCLOG("Corn initialized with seed texture");
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
        // ��һ�У��ӵ�һ�����ӿ�ʼ��ÿ��״̬ռ16x16����
        this->setTextureRect(Rect(16 * (stage + 1), 0, 16, 16));
        CCLOG("Updated corn growth stage to: %d, texture rect: x=%d", stage, 16 * (stage + 1));
    }
}