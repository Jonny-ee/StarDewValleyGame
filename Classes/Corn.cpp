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
        CCLOG("Created corn at position: (%.1f, %.1f)", position.x, position.y);
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

    // 直接设置纹理和初始区域
    this->setTexture("Plants.png");
    this->setTextureRect(Rect(16, 0, 16, 16));
    this->setScale(2.0f);

    CCLOG("Corn initialized with texture");
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
        this->setTextureRect(Rect(stage * 16, 0, 16, 16));
        CCLOG("Updated corn growth stage to: %d", stage);
    }
}