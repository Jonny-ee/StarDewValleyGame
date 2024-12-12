#include "GameTime.h"

GameTime* GameTime::_instance = nullptr;


void GameTime::updateYears()
{
    if (_Month == 13)
    {
        _Year++;
        _Month = 1;
    }
}
void GameTime::updateMonths()
{
    if (_Day == 31)
    {
        _Month++;
        _Day = 1;
    }
}
void GameTime::updateDays()
{
    if (_Hour == 24)
    {
        _Day++;
        _Hour = 0;
    }
}
void GameTime::updateHours()
{
    if (_Minute == 60)
    {
        _Hour++;
        _Minute = 0;
    }
}
//初始时间设置为第1年的3月1日的早上6点,设置每个月份固定为30天
GameTime::GameTime()
{
    startTime = std::clock();
    absoluteTimeInTenMinutes = 0;
    _Year = 1;
    _Month = 3;
    _Day = 1;
    _Minute = 0;
    _Hour = 6;
}
void GameTime::update()
{
    std::clock_t currentTime = std::clock();
    float elapsedSystemTime = static_cast<float>(currentTime - startTime) / CLOCKS_PER_SEC;
    // 计算本次时间增量，以每10分钟为单位
    int timeIncrementInTenMinutes = static_cast<int>((elapsedSystemTime / 9) * 10);
    absoluteTimeInTenMinutes += timeIncrementInTenMinutes;
    _Minute += timeIncrementInTenMinutes;
    updateHours();
    updateDays();
    updateMonths();
    updateYears();
}
int GameTime::getMinute()
{
    return _Minute;
}
int GameTime::getYear() {
    return _Year;
}
int GameTime::getHour() {
    return  _Hour;
}
int GameTime::getMonth()
{
    return _Month;
}
int  GameTime::getAbsoluteTime() {
    return absoluteTimeInTenMinutes;
}