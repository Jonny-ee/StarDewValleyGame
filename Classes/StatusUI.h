#pragma once
#include "cocos2d.h"
#include "GameTime.h"

class StatusUI : public cocos2d::Node {
public:
    static StatusUI* create();
    virtual bool init() override;

    void update(float dt);

private:
    cocos2d::Sprite* _bgSprite;        // 背景图片
    cocos2d::Label* _timeLabel;         // 时间显示
    cocos2d::Label* _dateLabel;         // 日期显示

    void initBackground();
    void initTimeDisplay();
    void updateTimeDisplay();
};