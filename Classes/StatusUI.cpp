#include "StatusUI.h"

USING_NS_CC;

StatusUI* StatusUI::create() {
    auto ui = new (std::nothrow) StatusUI();
    if (ui && ui->init()) {
        ui->autorelease();
        return ui;
    }
    CC_SAFE_DELETE(ui);
    return nullptr;
}

bool StatusUI::init() {
    if (!Node::init()) {
        return false;
    }

    initBackground();
    initTimeDisplay();

    // 开启更新
    this->scheduleUpdate();

    return true;
}

void StatusUI::initBackground() {
    // 加载背景图片
    _bgSprite = Sprite::create("status_bg.png");  // 使用你的美术资源
    if (_bgSprite) {
        _bgSprite->setAnchorPoint(Vec2(1, 1));  // 右上角对齐
        this->addChild(_bgSprite);

        // 可以根据需要设置背景图片的缩放
        // _bgSprite->setScale(2.0f);
    }
}

void StatusUI::initTimeDisplay() {
    // 创建时间标签
    _timeLabel = Label::createWithSystemFont("", "Arial", 20);
    if (_timeLabel) {
        _timeLabel->setAnchorPoint(Vec2(1, 1));
        _timeLabel->setPosition(_bgSprite->getContentSize().width - 10,
            _bgSprite->getContentSize().height - 10);
        _timeLabel->setTextColor(Color4B::BLACK);
        _bgSprite->addChild(_timeLabel);
    }

    // 创建日期标签
    _dateLabel = Label::createWithSystemFont("", "Arial", 20);
    if (_dateLabel) {
        _dateLabel->setAnchorPoint(Vec2(1, 1));
        _dateLabel->setPosition(_bgSprite->getContentSize().width - 10,
            _bgSprite->getContentSize().height - 40);
        _dateLabel->setTextColor(Color4B::BLACK);
        _bgSprite->addChild(_dateLabel);
    }
}

void StatusUI::update(float dt) {
    updateTimeDisplay();
}

void StatusUI::updateTimeDisplay() {
    auto gameTime = GameTime::getInstance();

    // 更新时间显示
    if (_timeLabel) {
        char timeStr[32];
        sprintf(timeStr, "%02d:%02d", gameTime->getHour(), gameTime->getMinute());
        _timeLabel->setString(timeStr);
    }

    // 更新日期显示
    if (_dateLabel) {
        char dateStr[32];
        sprintf(dateStr, "Year %d Month %d Day %d",
            gameTime->getYear(),
            gameTime->getMonth(),
            gameTime->getDay());
        _dateLabel->setString(dateStr);
    }
}