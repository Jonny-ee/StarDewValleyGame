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
        corn->updateWaterStatus(2);  // ���ó�ʼˮ��״̬
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

    // ����ԭ�е������ʼ��
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 0, 16, 16));  // ����״̬
    this->setScale(2.0f);

    // ���ˮ��״̬����
    _waterStatusSprite = Sprite::create("TileSheets/emotes.png");
    if (!_waterStatusSprite) {
        CCLOG("Failed to create water status sprite!");
        return false;
    }
    _waterStatusSprite->setScale(1.0f);
    this->addChild(_waterStatusSprite, 2);
    _waterStatusSprite->setPosition(Vec2(8, 20));

    // ���ó�ʼˮ��״̬�����㣩
    Rect rect(0, 16, 16, 16);
    _waterStatusSprite->setTextureRect(rect);

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

/*
 *  ����ׯ��ˮ��״̬
 *  @param waterLevel ˮ��״̬(0-2������ȱˮ��ȱˮ��ˮ�ֳ���)
 */
void Corn::updateWaterStatus(int waterLevel)
{
    if (!_waterStatusSprite)
        return;

    Rect rect;
    switch (waterLevel)
    {
        case 2:  // ˮ�ֳ���
            rect = Rect(0, 16, 16, 16);
            break;
        case 1:  // ȱˮ
            rect = Rect(16, 16, 16, 16);
            break;
        case 0:  // ����ȱˮ
            rect = Rect(32, 16, 16, 16);
            break;
        default:
            CCLOG("Invalid water level: %d", waterLevel);
            return;
    }

    _waterStatusSprite->setTextureRect(rect);
}