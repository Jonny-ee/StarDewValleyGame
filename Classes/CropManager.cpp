#include "CropManager.h"

// ��ʼ����̬��Ա����
CropManager* CropManager::_instance = nullptr;  // ʵ��

/*
 * ��ȡũ�������ϵͳ����ʵ��
 * @return ũ�������ϵͳʵ��ָ��
 */
CropManager* CropManager::getInstance()
{
    if (!_instance)  // ���ʵ�������ڣ�����ʵ��
    {
        _instance = new CropManager();
    }
    return _instance;
}

/*
 * ���ָ��λ���Ƿ���Կ���
 * @param tilePos Ҫ����ͼƬ����
 * @return ������Կ��ѷ���true�����򷵻�false
 */
bool CropManager::canTill(const Vec2& tilePos) const
{
    if (!_gameMap)  // ����ͼ�Ƿ����
        return false;

    if (_gameMap->getMapName() != "Farm")  // ֻ����Farm��ͼ����
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // ��ȡ����ͼ��
    if (!backLayer)  // ���ͼ���Ƿ����
        return false;

    int tileGID = backLayer->getTileGIDAt(tilePos);  // ��ȡָ��λ�õ�ͼ��ID

    return std::find(TILLABLE_TILES.begin(), TILLABLE_TILES.end(), tileGID) != TILLABLE_TILES.end();  // ����Ƿ��ǿɿ���ͼ��
}

/*
 * ����ָ��λ�õ�����
 * @param tilePos Ҫ���ѵ�ͼƬ����
 * @return ���ѳɹ�����true��ʧ�ܷ���false
 */
bool CropManager::tillSoil(const Vec2& tilePos)
{
    if (!canTill(tilePos))  // ����Ƿ���Կ���
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // ��ȡ����ͼ��
    if (!backLayer)  // ���ͼ���Ƿ����
        return false;

    backLayer->setTileGID(TILLED_TILE_ID, tilePos);  // ��ͼ������Ϊ�ѿ���״̬

    return true;
}

/*
 * ���ָ��λ���Ƿ�����Ƴ���Դ
 * @param tilePos Ҫ����ͼ������
 * @return ��������Ƴ���Դ����true�����򷵻�false
 */
bool CropManager::resourceCanRemove(const Vec2& tilePos) const
{
    if (!_gameMap)  // ����ͼ�Ƿ����
        return false;

    if (_gameMap->getMapName() != "Farm")  // ֻ����Farm��ͼ������ʯ
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // ��ȡresourceͼ��
    if (!backLayer)  // ���ͼ���Ƿ����
        return false;

    int tileGID = backLayer->getTileGIDAt(tilePos);  // ��ȡָ��λ�õ�ͼ��ID

    return std::find(RESOURCE_TILES.begin(), RESOURCE_TILES.end(), tileGID) != RESOURCE_TILES.end();  // ����Ƿ��ǿ��Ƴ�����Դͼ��
}


/*
 * �Ƴ�ָ��λ�õ���Դ
 * @param tilePos Ҫ�Ƴ���Դ��ͼ������
 * @return ����ɹ��Ƴ�����true�����򷵻�false
 */
bool CropManager::removeResource(const Vec2& tilePos)
{
    if (!resourceCanRemove(tilePos))  // ����Ƿ�����Ƴ���Դ
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // ��ȡ����ͼ��
    if (!backLayer)  // ���ͼ���Ƿ����
        return false;

    // ������Դͼ�㣬����Դͼ���滻Ϊ���Ƴ���ͼ��
    backLayer->setTileGID(RESOURCE_REMOVED_TILE_ID, tilePos);

    return true;
}

/*
 * ���ָ��λ���Ƿ���Խ�ˮ
 */
bool CropManager::canWater(const Vec2& tilePos) const
{
    if (!_gameMap || _gameMap->getMapName() != "Farm")
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // ����Ƿ����ѿ��ѵ�����
    return backLayer->getTileGIDAt(tilePos) == TILLED_TILE_ID;
}

/*
 * ��ˮָ��λ�õ�����
 */
bool CropManager::waterSoil(const Vec2& tilePos)
{
    if (!canWater(tilePos))
        return false;
    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;
    // ��ȡĿ����Ƭ����
    Sprite* tile = backLayer->getTileAt(tilePos);
    if (!tile)
        return false;
    // ������ˮЧ��
    createWaterEffect(tile);
    // ��ʾ��ˮ��ʾ
    showWateringPopup();
    // ���������ˮ��״̬
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        if (_cropInfos[i].tilePos == tilePos)
        {
            CCLOG("Watering crop at position (%f, %f)", tilePos.x, tilePos.y);
            _cropInfos[i].isWatered = true;
            _cropInfos[i].waterLevel = 2;  // �ָ�������״̬
            if (i < _crops.size() && _crops[i])
            {
                if (auto corn = dynamic_cast<Corn*>(_crops[i]))
                {
                    CCLOG("Updating water status for corn");
                    corn->updateWaterStatus(2);
                }
                else if (auto tomato = dynamic_cast<Tomato*>(_crops[i]))
                {
                    CCLOG("Updating water status for tomato");
                    tomato->updateWaterStatus(2);
                }
            }
            break;
        }
    }
    return true;
}

/*
 * ������ˮЧ��
 */
void CropManager::createWaterEffect(Sprite* tile)
{
    // ���ý�ˮ�����ɫ
    tile->setColor(WATER_COLOR);

    // �����ָ�ԭɫ�Ķ�������
    auto delay = DelayTime::create(WATER_DURATION);
    auto fadeBack = TintTo::create(FADE_DURATION, 255, 255, 255);
    tile->runAction(Sequence::create(delay, fadeBack, nullptr));
}

/*
 * ��ʾ��ˮ��ʾ
 */
void CropManager::showWateringPopup()
{
    if (!_gameScene || !_gameMap) return;

    // ��ȡ���λ��
    auto player = Player::getInstance();
    if (!player) return;
    Vec2 playerPos = player->getPosition();

    // ������͸������
    auto popupBg = LayerColor::create(Color4B(0, 0, 0, 150), 200, 80);

    // �����ı���ǩ
    auto label = Label::createWithSystemFont("Watering!", "Arial", 24);
    label->setPosition(Vec2(100, 40));
    label->setColor(Color3B::WHITE);
    popupBg->addChild(label);

    // ���õ���λ�õ����ͷ��
    popupBg->setPosition(Vec2(
        playerPos.x - 100,  // ������ʾ
        playerPos.y + 50    // ������Ϸ�
    ));

    // ��ӵ���Ϸ����
    _gameScene->addChild(popupBg, 1000);

    // ������������
    popupBg->setScale(0);
    popupBg->runAction(Sequence::create(
        ScaleTo::create(0.2f, 1.0f),
        DelayTime::create(1.0f),
        FadeOut::create(0.2f),
        RemoveSelf::create(),
        nullptr
    ));

    // ��ӵ��������
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [popupBg](Touch* touch, Event* event) {
        popupBg->runAction(Sequence::create(
            FadeOut::create(0.2f),
            RemoveSelf::create(),
            nullptr
        ));
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, popupBg);
}

/*
 * ������갴���¼���ִ�п��Ѳ���
 * @param mousePos ���λ��
 * @param player ��Ҷ���ָ��
 */
void CropManager::onMouseDown(const Vec2& mousePos, Player* player)
{
    if (!player || !_gameMap)               // ����Ҫ����
    {
        return;
    }

    if (_gameMap->getMapName() != "Farm")   // ȷ��ֻ��Farm��ͼ��ִ��
    {
        return;
    }

    Vec2 playerPos = player->getPosition();                         // ��ȡ���λ��
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);   // ת��Ϊ��Ƭ����

    // ���ݵ�ǰ����ִ����Ӧ����
    if (player->getCurrentTool() == Player::ToolType::SHOVEL)
    {
        if (canTill(playerTilePos))                 // ����Ƿ���Կ���
        {
            if (tillSoil(playerTilePos))            // ���Կ�������
            {
                player->performAction(mousePos);    // ִ�п��Ѷ���
            }
        }
    }
    else if (player->getCurrentTool() == Player::ToolType::WATERING)
    {
        if (canWater(playerTilePos))                // ����Ƿ���Խ�ˮ
        {
            if (waterSoil(playerTilePos))           // ���Խ�ˮ
            {
                player->performAction(mousePos);    // ִ�н�ˮ����
            }
        }
    }
}

/*
 * ��ʾ��ʾ�ı�
 * @param text Ҫ��ʾ���ı�
 * @param tilePos ��Ƭ����
 * @param duration ��ʾ����ʱ�䣨0��ʾ������ʾ��
 */
void CropManager::showTip(const std::string& text, const Vec2& tilePos, float duration) const
{
    if (!tipLabel)
    {
        tipLabel = Label::createWithSystemFont(text, "Arial", 20);
        if (tipLabel)
        {
            tipLabel->setTextColor(Color4B::WHITE);
            Director::getInstance()->getRunningScene()->addChild(tipLabel, 10);
        }
    }

    tipLabel->setString(text);
    tipLabel->setVisible(true);

    // ����λ���ڸ����Ϸ�
    if (_gameMap)
    {
        // ֱ��ʹ�� GameMap ������ת������
        Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
        // ����������Ļ������ʵ�ƫ��
        tipLabel->setPosition(Vec2(worldPos.x + 20, worldPos.y + 40));
    }

    if (duration > 0)
    {
        tipLabel->stopAllActions();  // ֹ֮ͣǰ�Ķ���
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

/*
 * ������ʾ�ı�
 */
void CropManager::hideTip() const
{
    if (tipLabel)
    {
        tipLabel->setVisible(false);
    }
}

/*
 * ���ָ����Ƭλ���Ƿ���������
 * @param tilePos Ҫ������Ƭ����
 * @return �����λ���������ﷵ��true�����򷵻�false
 */
bool CropManager::hasCropAt(const Vec2& tilePos) const
{
    // ����λ���Ƿ���������
    for (const auto& info : _cropInfos)
    {
        if (info.tilePos == tilePos)
        {
            return true;
        }
    }
    return false;
}

/*
 * ʵʱ����ũ���������ʾ
 * @param playerTilePos ������ڵ���Ƭ����
 * @param playerTool ��ҵ�ǰ����
 */
void CropManager::updateTips(const Vec2& playerTilePos, Player::ToolType playerTool) const
{
    // �������ũ����ͼ������ʾ��ʾ
    if (!_gameMap || _gameMap->getMapName() != "Farm")
    {
        hideTip();
        return;
    }
    // ����λ���Ƿ��Ǹ���
    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
    {
        hideTip();
        return;
    }
    int tileGID = backLayer->getTileGIDAt(playerTilePos);

    // �������ڸ�����
    if (playerTool == Player::ToolType::NONE && tileGID == TILLED_TILE_ID && !hasCropAt(playerTilePos))
    {
        showTip("Press P to plant", playerTilePos);
        return;
    }

    // װ����ͷ����������ĵؿ���
    if (playerTool == Player::ToolType::AXE && hasCropAt(playerTilePos))
    {
        // ��������Ƿ����
        for (const auto& crop : _cropInfos)
        {
            if (crop.tilePos == playerTilePos && crop.growthStage == FINAL_GROWTH_STAGE)
            {
                showTip("Press H to harvest", playerTilePos);
                return;
            }
        }
    }
    // ����Ƿ����ʩ��
    if (canFertilize(playerTilePos) &&
        ItemSystem::getInstance()->getItemCount("fertilizer") > 0)
    {
        showTip("Press F to fertilize", playerTilePos);
        return;
    }
    // �������������ʾ�����˵�ǰλ����������������ظ���ֲ��
    if (!hasCropAt(playerTilePos))
    {
        hideTip();
    }
}

/*
 * ���ָ��λ���Ƿ������ֲ
 * @param tilePos Ҫ������Ƭ����
 * @return ���������ֲ����true�����򷵻�false
 */
bool CropManager::canPlant(const Vec2& tilePos) const
{
    if (!_gameMap || _gameMap->getMapName() != "Farm")
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // ����Ƿ���������
    if (hasCropAt(tilePos))
    {
        showTip("Cannot plant here again!", tilePos);
        return false;
    }

    // ����Ƿ����Ѹ��ػ��ѽ�ˮ�ĸ���
    int tileGID = backLayer->getTileGIDAt(tilePos);
    return tileGID == TILLED_TILE_ID;
}

/*
 * ��ָ��λ����ֲ����
 * @param tilePos Ҫ��ֲ����Ƭ����
 * @return ��ֲ�ɹ�����true�����򷵻�false
 */
bool CropManager::plantCorn(const Vec2& tilePos)
{
    if (!canPlant(tilePos))
    {
        return false;
    }

    // �����ұ������Ƿ�����������
    auto itemSystem = ItemSystem::getInstance();
    if (!itemSystem->hasEnoughItems("corn seed", 1))
    {
        CCLOG("No corn seeds available");
        return false;
    }

    // �۳�һ����������
    if (!itemSystem->removeItem("corn seed", 1))
    {
        return false;
    }

    // ��ȡ�������겢Ӧ��ƫ��
    Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
    worldPos.x += CROP_OFFSET_X;
    worldPos.y += CROP_OFFSET_Y;

    // ������������
    auto corn = Corn::create(worldPos);
    if (corn)
    {
        _gameScene->addChild(corn, 0);
        _crops.push_back(corn);

        // ��¼������Ϣ
        CropInfo info;
        info.position = worldPos;
        info.tilePos = tilePos;
        info.growthStage = 0;
        info.type = "corn";

        // ��¼��ֲʱ��
        auto gameTime = GameTime::getInstance();
        info.plantDay = gameTime->getDay();
        info.plantMonth = gameTime->getMonth();
        info.plantYear = gameTime->getYear();
        info.waterLevel = 1;        // ��ʼ��Ϊ��Ϊȱˮ����Ҫ��ˮ�ڶ���Ż᳤��
        info.isWatered = false;     // ��ֲʱ��Ϊδ��ˮ

        _cropInfos.push_back(info);

        CCLOG("Corn planted on day %d, month %d, year %d",
            info.plantDay, info.plantMonth, info.plantYear);
        return true;
    }

    return false;
}

/*
 * ��ָ��λ����ֲ����
 * @param tilePos Ҫ��ֲ����Ƭ����
 * @return ��ֲ�ɹ�����true�����򷵻�false
 */
bool CropManager::plantTomato(const Vec2& tilePos)
{
    if (!canPlant(tilePos))
    {
        return false;
    }
    // �����ұ������Ƿ��з�������
    auto itemSystem = ItemSystem::getInstance();
    if (!itemSystem->hasEnoughItems("tomato seed", 1))
    {
        CCLOG("No tomato seeds available");
        return false;
    }
    // �۳�һ����������
    if (!itemSystem->removeItem("tomato seed", 1))
    {
        return false;
    }
    // ��ȡ�������겢Ӧ��ƫ��
    Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
    worldPos.x += CROP_OFFSET_X;
    worldPos.y += CROP_OFFSET_Y;
    // ������������
    auto tomato = Tomato::create(worldPos);
    if (tomato)
    {
        _gameScene->addChild(tomato, 0);
        _crops.push_back(tomato);
        // ��¼������Ϣ
        CropInfo info;
        info.position = worldPos;
        info.tilePos = tilePos;
        info.growthStage = 0;
        info.type = "tomato";
        // ��¼��ֲʱ��
        auto gameTime = GameTime::getInstance();
        info.plantDay = gameTime->getDay();
        info.plantMonth = gameTime->getMonth();
        info.plantYear = gameTime->getYear();
        info.waterLevel = 1;        // ��ʼ��Ϊ��Ϊȱˮ
        info.isWatered = false;     // ��ֲʱ��Ϊδ��ˮ
        info.growthCounter = 0;     // �ɳ���������ʼ��Ϊ0
        _cropInfos.push_back(info);
        CCLOG("Tomato planted on day %d, month %d, year %d",
            info.plantDay, info.plantMonth, info.plantYear);
        return true;
    }
    return false;
}

/*
 * ���ݵ�ǰ�����������ж���ֲ����
 * @param tilePos Ҫ��ֲ����Ƭ����
 * @return װ����Ч���ӷ���true�����򷵻�false
 */
bool CropManager::plantCrop(const Vec2& tilePos)
{
    auto player = Player::getInstance();
    if (!player)
        return false;
    // ���ݵ�ǰѡ���������������ֲ
    switch (player->getCurrentSeed())
    {
        case Player::SeedType::CORN:
            return plantCorn(tilePos);
        case Player::SeedType::TOMATO:
            return plantTomato(tilePos);
        case Player::SeedType::NONE:
        default:
            return false;
    }
}

/*
 * ��ʼ�����̼�����
 * ����P��������ֲ����
 */
void CropManager::initKeyboardListener()
{
    if (_keyboardListener)
        return;
    _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
        {
            auto player = Player::getInstance();
            if (!player)
                return;

            // ��ȡ���λ��
            Vec2 playerPos = player->getPosition();
            Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);
            if (keyCode == EventKeyboard::KeyCode::KEY_P)  // P����ֲ
            {
                if (player->getCurrentTool() == Player::ToolType::NONE)
                {
                    if (plantCrop(playerTilePos))
                    {
                        CCLOG("Successfully planted crop");
                    }
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_H)  // H���ջ�
            {
                if (player->getCurrentTool() == Player::ToolType::AXE)  // ��Ҫװ����ͷ
                {
                    CCLOG("Trying to harvest with axe...");
                    if (canHarvest(playerTilePos))
                    {
                        if (harvestCrop(playerTilePos))
                        {
                            CCLOG("Successfully harvested crop");
                        }
                    }
                    else
                    {
                        CCLOG("Cannot harvest at this position");
                    }
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_F)  // F��ʩ��
            {
                auto player = Player::getInstance();
                if (!player)
                    return;

                Vec2 playerPos = player->getPosition();
                Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);

                if (canFertilize(playerTilePos))
                {
                    if (fertilizeCrop(playerTilePos))
                    {
                        CCLOG("Successfully fertilized crop");
                    }
                }
            }
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        _keyboardListener, _gameMap->getTileMap());
}

/*
 * �����������������״̬
 * ÿ�����һ��
 */
void CropManager::updateCrops()
{
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        auto& info = _cropInfos[i];
        // �ȼ��ˮ��״̬
        if (info.waterLevel <= 0)
        {
            if (i < _crops.size() && _crops[i])
            {
                _crops[i]->removeFromParent();
                _crops.erase(_crops.begin() + i);
            }
            _cropInfos.erase(_cropInfos.begin() + i);
            i--;
            continue;
        }
        // �������������ж��Ƿ��������
        // ���׵������߼�
        if (info.type == "corn")
        {
            // ����ֻ��Ҫ��ˮ��������
            if (info.isWatered &&
                info.growthStage < Corn::getTotalGrowthStages() - 1)
            {
                info.growthStage++;
                if (i < _crops.size() && _crops[i])
                {
                    if (auto corn = dynamic_cast<Corn*>(_crops[i]))
                    {
                        corn->updateGrowthStage(info.growthStage);
                    }
                }
            }
        }
        // ���ѵ������߼�
        else if (info.type == "tomato")
        {
            // ȷ�� growthCounter ������ָ���
            if (info.growthCounter > 1000000) {  // ��������쳣�������
                info.growthCounter = 0;  // ����Ϊ0
            }
            if (info.isWatered)  // ���콽��ˮ
            {
                info.growthCounter++;
                CCLOG("Tomato at position (%.1f, %.1f) - Growth counter increased to: %u",
                    info.position.x, info.position.y, info.growthCounter);
                // ֻ�е��������ﵽ2ʱ������
                if (info.growthCounter == 2 &&
                    info.growthStage < Tomato::getTotalGrowthStages() - 1)
                {
                    info.growthStage++;
                    info.growthCounter = 0;  // ���������ü�����
                    CCLOG("Tomato growing to stage: %d", info.growthStage);
                    if (i < _crops.size() && _crops[i])
                    {
                        if (auto tomato = dynamic_cast<Tomato*>(_crops[i]))
                        {
                            tomato->updateGrowthStage(info.growthStage);
                        }
                    }
                }
            }
            else if (info.waterLevel < 2)  // ֻ�е�ˮ�ֲ���ʱ�����ü�����
            {
                info.growthCounter = 0;
                CCLOG("Tomato growth counter reset due to insufficient water level");
            }
        }

        // ����ˮ��״̬
        info.waterLevel--;
        // ����ˮ��״̬��ʾ
        if (i < _crops.size() && _crops[i])
        {
            if (auto corn = dynamic_cast<Corn*>(_crops[i]))
            {
                corn->updateWaterStatus(info.waterLevel);
            }
            else if (auto tomato = dynamic_cast<Tomato*>(_crops[i]))
            {
                tomato->updateWaterStatus(info.waterLevel);
            }
        }
        // ���ý�ˮ״̬
        info.isWatered = false;
    }
}

/*
 * ���浱ǰ��ͼ�������������Ϣ
 */
void CropManager::saveCrops()
{
    std::vector<CropInfo> newCropInfos;
    for (auto crop : _crops)
    {
        if (crop)
        {
            CropInfo info;
            info.position = crop->getPosition();
            info.tilePos = _gameMap->convertToTileCoord(crop->getPosition());

            // �����е� _cropInfos �в��Ҷ�Ӧλ�õ�������Ϣ
            auto it = std::find_if(_cropInfos.begin(), _cropInfos.end(),
                [&info](const CropInfo& existingInfo) {
                    return existingInfo.position == info.position;
                });
            if (it != _cropInfos.end())
            {
                // ����ҵ��ˣ�ʹ�����е�������Ϣ
                info = *it;  // ����������Ϣ������ growthCounter
            }
            else
            {
                info.growthStage = 0;
                auto gameTime = GameTime::getInstance();
                info.plantDay = gameTime->getDay();
                info.plantMonth = gameTime->getMonth();
                info.plantYear = gameTime->getYear();
                info.waterLevel = 1;
                info.isWatered = false;
                info.growthCounter = 0;  // ��ʼ��������

                // ����������������
                if (dynamic_cast<Corn*>(crop))
                {
                    info.type = "corn";
                }
                else if (dynamic_cast<Tomato*>(crop))
                {
                    info.type = "tomato";
                }
            }
            newCropInfos.push_back(info);
        }
    }
    _cropInfos = std::move(newCropInfos);
}

/*
 * ���ز����´������������
 */
void CropManager::loadCrops()
{
    // ֻ��Farm��ͼ��������
    if (!_gameMap || _gameMap->getMapName() != "Farm")
    {
        CCLOG("Not loading crops: not in Farm map");
        return;
    }
    clearCrops();
    for (const auto& info : _cropInfos)
    {
        if (info.type == "corn")
        {
            auto corn = Corn::create(info.position);
            if (corn)
            {
                _gameScene->addChild(corn, 0);
                _crops.push_back(corn);
                corn->updateGrowthStage(info.growthStage);
                corn->updateWaterStatus(info.waterLevel);
                CCLOG("Loaded corn at (%.1f, %.1f) with growth stage %d and water level %d",
                    info.position.x, info.position.y, info.growthStage, info.waterLevel);
            }
        }
        else if (info.type == "tomato")
        {
            auto tomato = Tomato::create(info.position);
            if (tomato)
            {
                _gameScene->addChild(tomato, 0);
                _crops.push_back(tomato);
                tomato->updateGrowthStage(info.growthStage);
                tomato->updateWaterStatus(info.waterLevel);
                CCLOG("Loaded tomato at (%.1f, %.1f) with growth stage %d and water level %d",
                    info.position.x, info.position.y, info.growthStage, info.waterLevel);
            }
        }
    }
}

/*
 * ����ǰ��ͼ�ϵ��������ﾫ��
 */
void CropManager::clearCrops()
{
    for (auto crop : _crops)
    {
        if (crop)
        {
            crop->removeFromParent();
        }
    }
    _crops.clear();
}

/*
 * ���ָ��λ���Ƿ�����ջ�
 * @param tilePos Ҫ������Ƭ����
 * @return ��������ѳ�������ջ񷵻�true�����򷵻�false
 */
bool CropManager::canHarvest(const Vec2& tilePos) const
{
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        if (_cropInfos[i].tilePos == tilePos && _cropInfos[i].growthStage == FINAL_GROWTH_STAGE)
        {
            return true;
        }
    }
    return false;
}

/*
 * �ջ�ָ��λ�õ�����
 * @param tilePos Ҫ�ջ����Ƭ����
 * @return �ջ�ɹ�����true�����򷵻�false
 */
bool CropManager::harvestCrop(const Vec2& tilePos)
{
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        if (_cropInfos[i].tilePos == tilePos && _cropInfos[i].growthStage == FINAL_GROWTH_STAGE)
        {
            // �����ջ����
            createHarvestDrop(_cropInfos[i].position);

            // ��ȡ��ǰ��ֲ�ȼ�
            auto skillSystem = SkillSystem::getInstance();
            int currentLevel = skillSystem->getSkillLevel(SkillType::FARMING);

            // ������ֲ����
            skillSystem->gainExp(SkillType::FARMING, 10);

            // ����Ƿ�ո�����3��
            if (currentLevel < 3 && skillSystem->getSkillLevel(SkillType::FARMING) >= 3)
            {
                // �ﵽ3������跬������
                auto itemSystem = ItemSystem::getInstance();
                itemSystem->addItem("tomato seed", 1);
                CCLOG("Unlocked tomato seed at farming level 3");
            }

            // �Ƴ�����
            if (i < _crops.size() && _crops[i])
            {
                _crops[i]->removeFromParent();
                _crops.erase(_crops.begin() + i);
            }
            _cropInfos.erase(_cropInfos.begin() + i);

            CCLOG("Harvested crop at position (%f, %f)", tilePos.x, tilePos.y);
            return true;
        }
    }
    return false;
}

/*
 * �����ջ��ĵ�����
 * @param position ���������ɵ���������λ��
 * ���ܣ�
 * 1.���������ﾫ��
 * 2.���õ�������ۺ�λ��
 * 3.����Զ�ʰȡ���
 */
void CropManager::createHarvestDrop(const Vec2& position)
{
    // ���������ﾫ��
    auto drop = Sprite::create("Plants.png");
    if (drop)
    {
        // ���������������õ�������ͼ����
        for (const auto& info : _cropInfos)
        {
            if (info.position == position)
            {
                if (info.type == "corn")
                {
                    drop->setTextureRect(Rect(80, 0, 16, 16));      // �����ջ���
                }
                else if (info.type == "tomato")
                {
                    drop->setTextureRect(Rect(80, 16, 16, 16));     // �����ջ���
                }
                break;
            }
        }
        // ����λ�ã���΢ƫ��һ�㣬������ȫ�ص���
        drop->setPosition(position + Vec2(30, 0));
        // �������ű���Ϊ2.0
        drop->setScale(2.0f);
        // ��ӵ�����
        _gameScene->addChild(drop);
        // ������ʱ����������
        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule([this, drop, position](float dt) {
            auto player = Player::getInstance();
            if (player)
            {
                // �������Ƿ��㹻����16�����ڣ�
                if (player->getPosition().distance(drop->getPosition()) < 16)
                {
                    // ��������������Ӷ�Ӧ���ջ���
                    auto itemSystem = ItemSystem::getInstance();
                    for (const auto& info : _cropInfos)
                    {
                        if (info.position == position)
                        {
                            if (info.type == "corn")
                            {
                                itemSystem->addItem("corn", 1);
                            }
                            else if (info.type == "tomato")
                            {
                                itemSystem->addItem("tomato", 1);
                            }
                            break;
                        }
                    }
                    // �Ƴ�������
                    drop->removeFromParent();
                    // ֹͣ��ʱ��
                    Director::getInstance()->getScheduler()->unschedule("check_drop_distance", drop);
                }
            }
            }, drop, 0.1f, false, "check_drop_distance");  // ÿ0.1����һ��
    }
}


bool CropManager::canFertilize(const Vec2& tilePos) const
{
    for (const auto& info : _cropInfos)
    {
        if (info.tilePos == tilePos && info.growthStage < FINAL_GROWTH_STAGE)
        {
            return true;
        }
    }
    return false;
}

bool CropManager::fertilizeCrop(const Vec2& tilePos)
{
    auto itemSystem = ItemSystem::getInstance();
    if (!itemSystem->hasEnoughItems("fertilizer", 1))
    {
        return false;
    }
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        if (_cropInfos[i].tilePos == tilePos && _cropInfos[i].growthStage < FINAL_GROWTH_STAGE)
        {
            // ���ķ���
            itemSystem->removeItem("fertilizer", 1);

            // ���������׶�
            _cropInfos[i].growthStage++;

            // ����������ʾ
            if (i < _crops.size() && _crops[i])
            {
                if (_cropInfos[i].type == "corn")
                {
                    if (auto corn = dynamic_cast<Corn*>(_crops[i]))
                    {
                        corn->updateGrowthStage(_cropInfos[i].growthStage);
                    }
                }
                else if (_cropInfos[i].type == "tomato")
                {
                    if (auto tomato = dynamic_cast<Tomato*>(_crops[i]))
                    {
                        tomato->updateGrowthStage(_cropInfos[i].growthStage);
                    }
                }
            }
            return true;
        }
    }
    return false;
}