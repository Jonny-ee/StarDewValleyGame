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
	int getTimeInTenMinutes();
	int getYear();
	int getHour();
private:
	std::clock_t startTime;
	GameTime();
	static GameTime* _instance;
	int gameTimeInTenMinutes;
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
