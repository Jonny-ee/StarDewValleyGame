#include "BaseEvent.h"
bool BaseEvent::init() {
    if (!Node::init()) {
        return false;
    }
    initKeyboardListener();
    this->scheduleUpdate();
    return true;
}
void BaseEvent::initKeyboardListener() {
    _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = CC_CALLBACK_1(BaseEvent::onKeyPressed, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);
}
void BaseEvent::update(float dt) {
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
void BaseEvent::showBlackFilter(float duration) {
    if (!_blackFilter && _gameMap) {
        // 创建覆盖整个地图的黑色滤镜
        Size mapSize = getMapSize();
        _blackFilter = LayerColor::create(Color4B(0, 0, 0, 0));
        _blackFilter->setContentSize(mapSize);

        // 设置位置与地图对齐
        _blackFilter->setPosition(_gameMap->getTileMap()->getPosition());

        // 添加到地图上，确保在最上层
        _gameMap->addChild(_blackFilter, 999);
    }

    if (_blackFilter) {
        _blackFilter->runAction(FadeTo::create(duration, 255));
    }
}

void BaseEvent::hideBlackFilter(float duration) {
    if (_blackFilter) {
        _blackFilter->runAction(Sequence::create(
            FadeTo::create(duration, 0),
            CallFunc::create([this]() {
                _blackFilter->removeFromParent();
                _blackFilter = nullptr;
                }),
            nullptr
        ));
    }
}
Size BaseEvent::getMapSize() const {
    if (!_gameMap || !_gameMap->getTileMap()) return Size::ZERO;

    // 计算地图实际大小
    auto mapSize = _gameMap->getTileMap()->getMapSize();
    auto tileSize = _gameMap->getTileMap()->getTileSize();
    float scale = _gameMap->getTileMap()->getScale();

    return Size(mapSize.width * tileSize.width * scale,
        mapSize.height * tileSize.height * scale);
}

void BaseEvent::showPrompt() {
    if (!_promptLabel) {
        _promptLabel = Label::createWithSystemFont(
            getPromptText(), "Arial", 24);

        Size visibleSize = Director::getInstance()->getVisibleSize();

        _promptLabel->setPosition(Vec2(
             visibleSize.width / 2,
             visibleSize.height /2));

        this->addChild(_promptLabel, 1000);
    }
    _promptLabel->setVisible(true);
}

void BaseEvent::hidePrompt() {
    if (_promptLabel) {
        _promptLabel->setVisible(false);
    }
}

void BaseEvent::onKeyPressed(EventKeyboard::KeyCode keyCode) {
    if (_isInTriggerArea && !_isExecuting &&
        keyCode == EventKeyboard::KeyCode::KEY_M) {
        hidePrompt();
        executeEvent();
    }
}