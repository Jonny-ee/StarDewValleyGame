#include "GameTime.h"

GameTime* GameTime::_instance = nullptr;


void GameTime::updateYears()
{
    if (_Month >= 13)
    {
        _Year++;
        _Month = 1;
    }
}
void GameTime::updateSeason()
{
    if (_Month >= 3 && _Month < 6)
        _Season = "Spring";
    else if (_Month >= 6 && _Month < 9)
        _Season = "Summmer";
    else if (_Month >= 9  && _Month < 12)
        _Season = "Fall";
    else if (_Month >= 12 && _Month < 3)
        _Season = "Winter";
}
void GameTime::updateMonths()
{
    if (_Day >= 31)
    {
        _Month++;
        _Day = 1;
    }
}
void GameTime::updateDays()
{
    if (_Hour >= 24)
    {
        _Day++;
        _Hour = 0;
    }
}
void GameTime::updateHours()
{
    if (_Minute >= 60)
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
    _Season = "Spring";
}
void GameTime::update()
{
    const std::clock_t currentTime = std::clock();
    const float elapsedSystemTime = static_cast<float>(currentTime - startTime) / CLOCKS_PER_SEC;
    // 计算本次时间增量，以每10分钟为单位
    const int newTimeInTenMinutes = static_cast<int>(elapsedSystemTime * (1.0f / 7.0f));

    // 如果时间有增加
    if (newTimeInTenMinutes > absoluteTimeInTenMinutes) {
        // 计算增加了多少个10分钟单位
        const  int increment = newTimeInTenMinutes - absoluteTimeInTenMinutes;
        absoluteTimeInTenMinutes = newTimeInTenMinutes;

        // 更新分钟（每个单位是10分钟）
        _Minute += increment * 10;
    }
    updateHours();
    updateDays();
    updateMonths();
    updateYears();
    updateSeason();
}

int GameTime::getYear() {
    return _Year;
}

int GameTime::getMonth()
{
    return _Month;
}

int GameTime::getDay()
{
    return _Day;
}

int GameTime::getHour() {
    return  _Hour;
}

int GameTime::getMinute()
{
    return _Minute;
}
std::string GameTime::getSeason() {
    return _Season;
}

int  GameTime::getAbsoluteTime() {
    return absoluteTimeInTenMinutes;
}

void  GameTime::modifyGameTime(int targetHour) {
    _Day++;//天数加1
    updateMonths();//更新月，年
    updateYears();
    _Hour = targetHour;//将第二天设定成6点0分
    _Minute = 0;
}