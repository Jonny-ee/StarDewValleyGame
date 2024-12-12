#pragma once
#include "cocos2d.h"

class GameTime {
public:
	static GameTime* getInstance() {
		if (_instance == nullptr)
		{
			_instance = new GameTime();
		}
		return _instance;
	}

	GameTime(const GameTime&) = delete;
	GameTime& operator=(const GameTime&) = delete;

	//系统功能接口
	void update();
	int getAbsoluteTime();
	int getMinute();
	int getYear();
	int getHour();
	int getMonth();
private:
	std::clock_t startTime;
	GameTime();
	static GameTime* _instance;
	// 用于记录绝对游戏时间，单位为每10分钟
	int absoluteTimeInTenMinutes;
	
	int _Minute;
	int _Year;
	int _Month;
	int _Day;
	int _Hour;
	//更新年，月，日，小时
	void updateYears();
	void updateMonths();
	void updateDays();
	void updateHours();
};
