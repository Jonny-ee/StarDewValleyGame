#include "Crop.h"

USING_NS_CC;

/*
 * ũ���ﹹ�캯��
 * @param cropName ũ��������
 * @param growTime ��������ʱ��
 * @param waterNeeded ��Ҫ��ˮ����
 * @param spriteName ũ���ﾫ��ͼƬ����
 */
Crop::Crop(const std::string& cropName, int growTime, int waterNeeded, const std::string& spriteName)
    : _cropName(cropName), _growTime(growTime), _waterNeeded(waterNeeded), _currentGrowthTime(0),
    _isWatered(false), _isMatured(false), _isHarvested(false), _plantDay(0) {
    // ���س�ʼ��ũ����ͼƬ
    this->initWithFile(spriteName);
}

// ��������
Crop::~Crop() {}

/*
 * ��ֲũ����
 * @param startDay ��ʼ��ֲ����Ϸ����
 * ���ܣ���ʼ��ũ����ĸ���״̬
 */
void Crop::plant(int startDay) {
    _plantDay = startDay;      // ��¼��ֲ������
    _currentGrowthTime = 0;    // ��������ʱ��
    _isWatered = false;        // ��ʼδ��ˮ
    _isMatured = false;        // ��ʼδ����
    _isHarvested = false;      // ��ʼδ�ջ�
}

/*
 * ��ˮ
 * ���ܣ�Ϊũ���ｽˮ�����½�ˮ״̬
 */
void Crop::water() {
    _isWatered = true;  // ����ѽ�ˮ
}

/*
 * ����Ƿ����
 * @return ����ũ�����Ƿ��ѳ���
 */
bool Crop::isMatured() const {
    return _isMatured;
}

/*
 * ����Ƿ�����ջ�
 * @return ����ũ�����Ƿ�����ջ��ѳ�����δ�ջ�
 */
bool Crop::canHarvest() const {
    return _isMatured && !_isHarvested;
}

/*
 * ����ũ����״̬
 * @param currentDay ��ǰ��Ϸ����
 * ���ܣ�����ʱ��ͽ�ˮ״̬����ũ���������״̬
 */
void Crop::update(int currentDay) {
    if (_isHarvested) return; // ����Ѿ��ջ񣬾Ͳ��ٸ�������״̬

    // �����ǰ����ʱ��С����������ʱ�䣬���Ѿ���ˮ����Ż��������
    if (_currentGrowthTime < _growTime) {
        if (_isWatered) {
            _currentGrowthTime++;
            _isWatered = false; // ÿ�����������ý�ˮ״̬
        }
    }

    // �ж��Ƿ����
    if (_currentGrowthTime >= _growTime) {
        _isMatured = true;
        // ����Ϊ����״̬��ͼ��
        this->setTexture("matured_crop.png");
    }
}

/*
 * �ջ�ũ����
 * ���ܣ�����ũ�����ջ������߼�
 */
void Crop::harvest() {
    if (canHarvest()) {
        _isHarvested = true;
        CCLOG("Harvested crop: %s", _cropName.c_str());
        // �����ջ���״̬ͼ��
        this->setTexture("harvested_crop.png");
    }
    else {
        CCLOG("Crop is not ready for harvest.");
    }
}

/*
 * ��ȡũ��������
 * @return ����ũ���������
 */
const std::string& Crop::getCropName() const {
    return _cropName;
}