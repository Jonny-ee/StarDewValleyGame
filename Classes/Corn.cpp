#include "Corn.h"

/*
 * 玉米作物构造函数
 * 初始化玉米的基本属性
 */
Corn::Corn() : Crop("Corn", 8, 1, "Plants.png") {}

/*
 * 创建玉米作物实例
 * @param position 玉米作物的世界坐标位置
 * @return 创建成功返回玉米实例指针，失败返回nullptr
 */
Corn* Corn::create(const Vec2& position)
{
    Corn* corn = new (std::nothrow) Corn();
    if (corn && corn->init())
    {
        corn->autorelease();
        corn->setPosition(position);
        corn->updateWaterStatus(2);  // 设置初始水分状态
        return corn;
    }
    CC_SAFE_DELETE(corn);
    return nullptr;
}

/*
 * 初始化玉米作物
 * 设置初始纹理和显示属性
 * @return 初始化成功返回true，失败返回false
 */
bool Corn::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // 保持原有的作物初始化
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 0, 16, 16));  // 种子状态
    this->setScale(2.0f);

    // 添加水分状态精灵
    _waterStatusSprite = Sprite::create("TileSheets/emotes.png");
    if (!_waterStatusSprite) {
        CCLOG("Failed to create water status sprite!");
        return false;
    }
    _waterStatusSprite->setScale(1.0f);
    this->addChild(_waterStatusSprite, 2);
    _waterStatusSprite->setPosition(Vec2(8, 20));

    // 设置初始水分状态（充足）
    Rect rect(0, 16, 16, 16);
    _waterStatusSprite->setTextureRect(rect);

    CCLOG("Corn initialized with seed texture");
    return true;
}

/*
 * 更新玉米生长阶段
 * @param stage 目标生长阶段(0-4: 种子、幼苗、生长期、成熟期、收获期)
 */
void Corn::updateGrowthStage(int stage)
{
    if (stage >= 0 && stage < TOTAL_GROWTH_STAGES)
    {
        // 第一行，从第一个格子开始，每个状态占16x16像素
        this->setTextureRect(Rect(16 * (stage + 1), 0, 16, 16));
        CCLOG("Updated corn growth stage to: %d, texture rect: x=%d", stage, 16 * (stage + 1));
    }
}

/*
 *  更新庄稼水分状态
 *  @param waterLevel 水分状态(0-2：严重缺水、缺水、水分充足)
 */
void Corn::updateWaterStatus(int waterLevel)
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