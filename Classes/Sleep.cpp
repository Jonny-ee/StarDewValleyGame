#include"Sleep.h"
#include"GameTime.h"

SleepEvent* SleepEvent::create(GameMap* gameMap, Player* player) {
    auto event = new (std::nothrow) SleepEvent();
    if (event && event->init()) {
        event->autorelease();
        event->_gameMap = gameMap;
        event->_player = player;
        return event;
    }
    CC_SAFE_DELETE(event);
    return nullptr;
}
bool SleepEvent::checkTriggerCondition() {
    if (!_player || !_gameMap || _isExecuting) return false;

    Vec2 playerTilePos = _gameMap->convertToTileCoord(_player->getPosition());
    return _gameMap->getMapName() == "House" &&
        std::abs(playerTilePos.x - TRIGGER_POS.x) < 0.5f &&
        std::abs(playerTilePos.y - TRIGGER_POS.y) < 0.5f;
}

void SleepEvent::executeEvent() {
    if (_isExecuting) return;
    _isExecuting = true;
    _player->setCanPerformAction(false);

    auto sequence = Sequence::create(
        CallFunc::create([this]() {
            Vec2 sleepPos = _gameMap->convertToWorldCoord(SLEEP_POS);
            _player->setPosition(sleepPos);
            this->showBlackFilter();
            }),

        DelayTime::create(2.0f),

        CallFunc::create([this]() {
            GameTime::getInstance()->modifyGameTime(6);
            Vec2 wakeupPos = _gameMap->convertToWorldCoord(WAKEUP_POS);
            _player->setPosition(wakeupPos);
            this->hideBlackFilter();
            }),

        CallFunc::create([this]() {
            this->finishEvent();
            }),

        nullptr
    );

    this->runAction(sequence);
}

void SleepEvent::finishEvent() {
    _player->setCanPerformAction(true);
    _isExecuting = false;
}