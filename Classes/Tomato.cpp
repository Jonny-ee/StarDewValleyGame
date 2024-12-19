#include "Tomato.h"

/*
* �������ﹹ�캯��
* ��ʼ�����ѵĻ�������
*/
Tomato::Tomato() : Crop("Tomato", 8, 1, "Plants.png") {}

/*
* ������������ʵ��
* @param position �����������������λ��
* @return �����ɹ����ط���ʵ��ָ�룬ʧ�ܷ���nullptr
*/
Tomato* Tomato::create(const Vec2& position)
{
    Tomato* tomato = new (std::nothrow) Tomato();
    if (tomato && tomato->init())
    {
        tomato->autorelease();
        tomato->setPosition(position);
        tomato->updateWaterStatus(1);  // ���ó�ʼˮ��״̬����Ϊȱˮ��
        return tomato;
    }
    CC_SAFE_DELETE(tomato);
    return nullptr;
}

/*
* ��ʼ����������
* ���ó�ʼ�������ʾ����
* @return ��ʼ���ɹ�����true��ʧ�ܷ���false
*/
bool Tomato::init()
{
    if (!Sprite::init())
    {
        return false;
    }
    // ����ԭ�е������ʼ��
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 16, 16, 16));  // ����״̬���ڶ��еڶ�������
    this->setScale(2.0f);
    // ���ˮ��״̬����
    _waterStatusSprite = Sprite::create("TileSheets/emotes.png");
    if (!_waterStatusSprite)
    {
        return false;
    }
    _waterStatusSprite->setScale(1.0f);
    this->addChild(_waterStatusSprite, 2);
    _waterStatusSprite->setPosition(Vec2(8, 20));
    return true;
}

/*
* ���·��������׶�
* @param stage Ŀ�������׶�(0-4: ���ӡ����硢�����ڡ������ڡ��ջ���)
*/
void Tomato::updateGrowthStage(int stage)
{
    if (stage >= 0 && stage < TOTAL_GROWTH_STAGES)
    {
        // �ڶ��У��ӵڶ������ӿ�ʼ��ÿ��״̬ռ16x16����
        this->setTextureRect(Rect(16 * (stage + 1), 16, 16, 16));
    }
}

/*
*  ���·���ˮ��״̬
*  @param waterLevel ˮ��״̬(0-2������ȱˮ��ȱˮ��ˮ�ֳ���)
*/
void Tomato::updateWaterStatus(int waterLevel)
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