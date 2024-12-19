#include"WeatherManager.h"



WeatherManager* WeatherManager::_instance = nullptr;

WeatherManager* WeatherManager::getInstance() {
    if (_instance == nullptr) {
        _instance = new (std::nothrow) WeatherManager();
    }
    return _instance;
}

WeatherManager::WeatherManager() : _currentWeather(nullptr) {}

WeatherManager::~WeatherManager() {
    CC_SAFE_RELEASE(_currentWeather);
}

void WeatherManager::setWeather(Weather* weather) {
    if (_currentWeather) {
        _currentWeather->setStatus();  // 翻转之前的天气
        _currentWeather->release();
    }
    weather->setStatus();  // 激活新的天气
    _currentWeather = weather;
    _currentWeather->retain();
}

Weather* WeatherManager::getCurrentWeather() {
    return _currentWeather;
}
void WeatherManager::randomRefreshWeather() {
    int randomNum = rand() % 10;  // 生成0到9的随机数,对应各种天气，目前设置为3种天气
    Weather* newWeather = nullptr;
    switch (randomNum) {
        case 0:
            newWeather = Rain::create();
            break;
        case 1:
            newWeather = Drought::create();
            break;
        case 2:
        case 3://留下后续天气的位置，未添加前都属于正常天气
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            newWeather = NormalWeather::create();
            break;
    }
    if (newWeather) {
        this->setWeather(newWeather);
    }

    std::string name = newWeather->getWeatherName();
   // CCLOG("Today is %s",&name);
}