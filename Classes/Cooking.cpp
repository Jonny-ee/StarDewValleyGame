#include "Cooking.h"
#include "ItemSystem.h"
#include "SkillSystem.h"
#include <string>

Cooking* Cooking::create(GameMap* gameMap, Player* player) {
    auto event = new (std::nothrow) Cooking();
    if (event && event->init()) {
        event->autorelease();
        event->_gameMap = gameMap;
        event->_player = player;
        return event;
    }
    CC_SAFE_DELETE(event);
    return nullptr;
}

bool Cooking::checkTriggerCondition() {
    if (!_player || !_gameMap || _isExecuting) return false;

    const Vec2 playerTilePos = _gameMap->convertToTileCoord(_player->getPosition());
    return _gameMap->getMapName() == "House" &&
        std::abs(playerTilePos.x - TRIGGER_POS.x) < 0.5f &&
        std::abs(playerTilePos.y - TRIGGER_POS.y) < 0.5f;
}

void Cooking::executeEvent() {
    if (!checkTriggerCondition()) return;
    _player->setCanPerformAction(false);
    _isExecuting = true;
    auto _itemSystem = ItemSystem::getInstance();
    if(_itemSystem->hasEnoughItems("apple",3)&& _itemSystem->hasEnoughItems("bread", 3))
    {
        auto _skillSystem = SkillSystem::getInstance();
        const int count = _skillSystem->getSkillLevel(SkillType::COOKING);
        _itemSystem->addItem("CatFood",count);
        std::string _text = "Congratulations! You got ";
        _text += std::to_string(count);
        _text+=" Cat Food";
        showTip(_text, 2.0f);
        _skillSystem->gainExp(SkillType::COOKING, 1);
    }
    else
    {
        showTip("Lack of Ingredients!",2.0f);
    }
    finishEvent();
}
void Cooking::finishEvent() {
    _player->setCanPerformAction(true);
    _isExecuting = false;
}

void Cooking::showTip(const std::string& text, float duration)
{
    if (!tipLabel) 
    {
        tipLabel = Label::createWithSystemFont(text, "Arial", 24);
        if (tipLabel)
        {
            tipLabel->setPosition(Vec2(0, 0));
            Director::getInstance()->getRunningScene()->addChild(tipLabel, 10);
        }
    }

    tipLabel->setString(text); 
    tipLabel->setVisible(true);

    if (duration > 0)   
    {
        auto sequence = Sequence::create(
            DelayTime::create(duration),
            CallFunc::create([this]() {
                hideTip();
                }),
            nullptr
        );
        tipLabel->runAction(sequence);
    }
}

void Cooking::hideTip()
{
    if (tipLabel)  
    {
        tipLabel->setVisible(false);
    }
}