#include "Tomato.h"

/*
* 番茄作物构造函数
* 初始化番茄的基本属性
*/
Tomato::Tomato() : Crop("Tomato", 8, 1, "Plants.png") {}

/*
* 创建番茄作物实例
* @param position 番茄作物的世界坐标位置
* @return 创建成功返回番茄实例指针，失败返回nullptr
*/
Tomato* Tomato::create(const Vec2& position)
{
    Tomato* tomato = new (std::nothrow) Tomato();
    if (tomato && tomato->init())
    {
        tomato->autorelease();
        tomato->setPosition(position);
        tomato->updateWaterStatus(1);  // 设置初始水分状态（较为缺水）
        return tomato;
    }
    CC_SAFE_DELETE(tomato);
    return nullptr;
}

/*
* 初始化番茄作物
* 设置初始纹理和显示属性
* @return 初始化成功返回true，失败返回false
*/
bool Tomato::init()
{
    if (!Sprite::init())
    {
        return false;
    }
    // 保持原有的作物初始化
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 16, 16, 16));  // 种子状态，第二行第二个格子
    this->setScale(2.0f);
    // 添加水分状态精灵
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
* 更新番茄生长阶段
* @param stage 目标生长阶段(0-4: 种子、幼苗、生长期、成熟期、收获期)
*/
void Tomato::updateGrowthStage(int stage)
{
    if (stage >= 0 && stage < TOTAL_GROWTH_STAGES)
    {
        // 第二行，从第二个格子开始，每个状态占16x16像素
        this->setTextureRect(Rect(16 * (stage + 1), 16, 16, 16));
    }
}

/*
*  更新番茄水分状态
*  @param waterLevel 水分状态(0-2：严重缺水、缺水、水分充足)
*/
void Tomato::updateWaterStatus(int waterLevel)
{
    if (!_waterStatusSprite)
        return;
    Rect rect;
    switch (waterLevel)
    {
        case 2:  // 水分充足
            rect = Rect(0, 16, 16, 16);
            break;
        case 1:  // 缺水
            rect = Rect(16, 16, 16, 16);
            break;
        case 0:  // 严重缺水
            rect = Rect(32, 16, 16, 16);
            break;
        default:
            CCLOG("Invalid water level: %d", waterLevel);
            return;
    }
    _waterStatusSprite->setTextureRect(rect);
}