#include "Crop.h"

USING_NS_CC;

Crop::Crop(const std::string& cropName, int growTime, int waterNeeded, const std::string& spriteName)
    : _cropName(cropName), _growTime(growTime), _waterNeeded(waterNeeded), _currentGrowthTime(0),
    _isWatered(false), _isMatured(false), _isHarvested(false), _plantDay(0) {
    // 加载初始的农作物图片
    this->initWithFile(spriteName);  // `initWithFile` 来加载图片，创建精灵
}

Crop::~Crop() {}

void Crop::plant(int startDay) {
    _plantDay = startDay;      // 记录种植的天数
    _currentGrowthTime = 0;    // 重置生长时间
    _isWatered = false;       // 初始没有浇水
    _isMatured = false;       // 初始未成熟
    _isHarvested = false;     // 初始未收获
}

void Crop::water() {
    _isWatered = true;  // 标记已浇水
}

bool Crop::isMatured() const {
    return _isMatured;  // 判断是否成熟
}

bool Crop::canHarvest() const {
    return _isMatured && !_isHarvested;  // 判断是否可以收获
}

void Crop::update(int currentDay) {
    if (_isHarvested) return; // 如果已经收获，就不再更新生长状态

    // 如果当前天数超过了种植天数，并且已经浇水，作物才会继续生长
    if (_currentGrowthTime < _growTime) {
        // 每天增加生长时间
        if (_isWatered) {
            _currentGrowthTime++;
            _isWatered = false; // 每次浇水后就重置水状态
        }
    }

    // 判断是否成熟
    if (_currentGrowthTime >= _growTime) {
        _isMatured = true;
        // 可以切换为成熟状态的图像
        this->setTexture("matured_crop.png");  // 成熟的农作物图像
    }
}

void Crop::harvest() {
    if (canHarvest()) {
        _isHarvested = true;  // 标记已收获
        // 在这里可以掉落物品或触发其他逻辑
        CCLOG("Harvested crop: %s", _cropName.c_str());
        // 例如，改变农作物的状态图像
        this->setTexture("harvested_crop.png");
    }
    else {
        CCLOG("Crop is not ready for harvest.");
    }
}

const std::string& Crop::getCropName() const {
    return _cropName;
}
