#include "Crop.h"

USING_NS_CC;

Crop::Crop(const std::string& cropName, int growTime, int waterNeeded, const std::string& spriteName)
    : _cropName(cropName), _growTime(growTime), _waterNeeded(waterNeeded), _currentGrowthTime(0),
    _isWatered(false), _isMatured(false), _isHarvested(false), _plantDay(0) {
    // ���س�ʼ��ũ����ͼƬ
    this->initWithFile(spriteName);  // `initWithFile` ������ͼƬ����������
}

Crop::~Crop() {}

void Crop::plant(int startDay) {
    _plantDay = startDay;      // ��¼��ֲ������
    _currentGrowthTime = 0;    // ��������ʱ��
    _isWatered = false;       // ��ʼû�н�ˮ
    _isMatured = false;       // ��ʼδ����
    _isHarvested = false;     // ��ʼδ�ջ�
}

void Crop::water() {
    _isWatered = true;  // ����ѽ�ˮ
}

bool Crop::isMatured() const {
    return _isMatured;  // �ж��Ƿ����
}

bool Crop::canHarvest() const {
    return _isMatured && !_isHarvested;  // �ж��Ƿ�����ջ�
}

void Crop::update(int currentDay) {
    if (_isHarvested) return; // ����Ѿ��ջ񣬾Ͳ��ٸ�������״̬

    // �����ǰ������������ֲ�����������Ѿ���ˮ������Ż��������
    if (_currentGrowthTime < _growTime) {
        // ÿ����������ʱ��
        if (_isWatered) {
            _currentGrowthTime++;
            _isWatered = false; // ÿ�ν�ˮ�������ˮ״̬
        }
    }

    // �ж��Ƿ����
    if (_currentGrowthTime >= _growTime) {
        _isMatured = true;
        // �����л�Ϊ����״̬��ͼ��
        this->setTexture("matured_crop.png");  // �����ũ����ͼ��
    }
}

void Crop::harvest() {
    if (canHarvest()) {
        _isHarvested = true;  // ������ջ�
        // ��������Ե�����Ʒ�򴥷������߼�
        CCLOG("Harvested crop: %s", _cropName.c_str());
        // ���磬�ı�ũ�����״̬ͼ��
        this->setTexture("harvested_crop.png");
    }
    else {
        CCLOG("Crop is not ready for harvest.");
    }
}

const std::string& Crop::getCropName() const {
    return _cropName;
}
