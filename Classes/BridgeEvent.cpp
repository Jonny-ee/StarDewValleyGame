#include "BridgeEvent.h"

void BridgeEvent::update(float dt) {
    if (_bridgeRepaired)
        return;
    bool inTriggerArea = checkTriggerCondition();

    // 进入/离开触发区域时更新提示
    if (inTriggerArea != _isInTriggerArea) {
        _isInTriggerArea = inTriggerArea;
        if (_isInTriggerArea) {
            showPrompt();
        }
        else {
            hidePrompt();
        }
    }
}
BridgeEvent* BridgeEvent::create(GameMap* gameMap, Player* player) {
    auto event = new (std::nothrow) BridgeEvent();
    if (event && event->init()) {
        event->autorelease();
        event->_gameMap = gameMap;
        event->_player = player;
        return event;
    }
    CC_SAFE_DELETE(event);
    return nullptr;
}

bool BridgeEvent::checkTriggerCondition() {
    if (!_player || !_gameMap || _isExecuting) return false;

    Vec2 playerTilePos = _gameMap->convertToTileCoord(_player->getPosition());
    return _gameMap->getMapName() == TRIGGER_MAP &&
        (std::abs(playerTilePos.x - TRIGGER_POS1.x) < 0.5f &&
            std::abs(playerTilePos.y - TRIGGER_POS1.y) < 0.5f) ||
        (std::abs(playerTilePos.x - TRIGGER_POS2.x) < 0.5f &&
            std::abs(playerTilePos.y - TRIGGER_POS2.y) < 0.5f) &&
        !_gameMap->isBridgeRepaired();
}

void BridgeEvent::executeEvent() {
    if (_isExecuting|| _bridgeRepaired) return;
    _isExecuting = true;
    _player->setCanPerformAction(false);

    auto sequence = Sequence::create(
        CallFunc::create([this]() {
            this->showBlackFilter(0.5f);
            }),

        DelayTime::create(1.0f),

        CallFunc::create([this]() {
            _gameMap->repairBridge();
            this->hideBlackFilter(0.5f);
            }),

        DelayTime::create(0.5f),

        CallFunc::create([this]() {
            this->finishEvent();
            }),

        nullptr
    );

    this->runAction(sequence);
    _bridgeRepaired = true;
}

void BridgeEvent::finishEvent() {
    _player->setCanPerformAction(true);
    _isExecuting = false;
}