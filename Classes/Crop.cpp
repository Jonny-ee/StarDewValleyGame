#include "Crop.h"

USING_NS_CC;

/*
 * 农作物构造函数
 * @param cropName 农作物名称
 * @param growTime 生长所需时间
 * @param waterNeeded 需要浇水次数
 * @param spriteName 农作物精灵图片名称
 */
Crop::Crop(const std::string& cropName, int growTime, int waterNeeded, const std::string& spriteName)
    : _cropName(cropName), _growTime(growTime), _waterNeeded(waterNeeded), _currentGrowthTime(0),
    _isWatered(false), _isMatured(false), _isHarvested(false), _plantDay(0) {
    // 加载初始的农作物图片
    this->initWithFile(spriteName);
}

// 析构函数
Crop::~Crop() {}

/*
 * 种植农作物
 * @param startDay 开始种植的游戏天数
 * 功能：初始化农作物的各项状态
 */
void Crop::plant(int startDay) {
    _plantDay = startDay;      // 记录种植的日期
    _currentGrowthTime = 0;    // 重置生长时间
    _isWatered = false;        // 初始未浇水
    _isMatured = false;        // 初始未成熟
    _isHarvested = false;      // 初始未收获
}

/*
 * 浇水
 * 功能：为农作物浇水，更新浇水状态
 */
void Crop::water() {
    _isWatered = true;  // 标记已浇水
}

/*
 * 检查是否成熟
 * @return 返回农作物是否已成熟
 */
bool Crop::isMatured() const {
    return _isMatured;
}

/*
 * 检查是否可以收获
 * @return 返回农作物是否可以收获（已成熟且未收获）
 */
bool Crop::canHarvest() const {
    return _isMatured && !_isHarvested;
}

/*
 * 更新农作物状态
 * @param currentDay 当前游戏天数
 * 功能：根据时间和浇水状态更新农作物的生长状态
 */
void Crop::update(int currentDay) {
    if (_isHarvested) return; // 如果已经收获，就不再更新生长状态

    // 如果当前生长时间小于所需生长时间，且已经浇水，则才会继续生长
    if (_currentGrowthTime < _growTime) {
        if (_isWatered) {
            _currentGrowthTime++;
            _isWatered = false; // 每次生长后重置浇水状态
        }
    }

    // 判断是否成熟
    if (_currentGrowthTime >= _growTime) {
        _isMatured = true;
        // 更换为成熟状态的图标
        this->setTexture("matured_crop.png");
    }
}

/*
 * 收获农作物
 * 功能：处理农作物收获的相关逻辑
 */
void Crop::harvest() {
    if (canHarvest()) {
        _isHarvested = true;
        CCLOG("Harvested crop: %s", _cropName.c_str());
        // 更换收获后的状态图标
        this->setTexture("harvested_crop.png");
    }
    else {
        CCLOG("Crop is not ready for harvest.");
    }
}

/*
 * 获取农作物名称
 * @return 返回农作物的名称
 */
const std::string& Crop::getCropName() const {
    return _cropName;
}