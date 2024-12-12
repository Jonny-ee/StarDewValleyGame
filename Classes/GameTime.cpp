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
    if (gameTimeInTenMinutes == 60)
    {
        _Hour++;
        gameTimeInTenMinutes = 0;
    }
}
//初始时间设置为第1年的3月1日的早上6点,设置每个月份固定为30天
GameTime::GameTime()
{
    startTime = std::clock();
    _Year = 1;
    _Month = 3;
    _Day = 1;
    _Hour = 6;
}
void GameTime::update()
{
    std::clock_t currentTime = std::clock();
    float elapsedSystemTime = static_cast<float>(currentTime - startTime) / CLOCKS_PER_SEC;
    // 计算游戏时间，游戏时间单位为10分钟，现实9秒对应游戏里10分钟
    gameTimeInTenMinutes = static_cast<int>((elapsedSystemTime / 9) * 10);
    
    updateHours();
    updateDays();
    updateMonths();
    updateYears();
}
int GameTime::getTimeInTenMinutes()
{
    return gameTimeInTenMinutes;
}
int GameTime::getYear() {
    return _Year;
}
int GameTime::getHour() {
    return  _Hour;
}