#include "CropManager.h"

CropManager* CropManager::_instance = nullptr;  // ��ʼ����̬��Ա����

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
    else if (player->getCurrentTool() == Player::ToolType::AXE)
    {
        if (resourceCanRemove(playerTilePos))                // ����Ƿ�����Ƴ���Դ
        {
            if (removeResource(playerTilePos))           // �����Ƴ�
            {
                player->performAction(mousePos);    // ִ�ж���
            }
        }
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
    {
        return false;
    }

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
    {
        return false;
    }

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
        info.waterLevel = 2;        // ��ʼˮ�ֳ���
        info.isWatered = false;     // ��ֲʱ��Ϊ�ѽ�ˮ

        _cropInfos.push_back(info);

        CCLOG("Corn planted on day %d, month %d, year %d",
            info.plantDay, info.plantMonth, info.plantYear);
        return true;
    }

    return false;
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
            if (!player) return;

            // ��ȡ���λ��
            Vec2 playerPos = player->getPosition();
            Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);

            if (keyCode == EventKeyboard::KeyCode::KEY_P)  // P����ֲ
            {
                if (player->getCurrentTool() == Player::ToolType::NONE)
                {
                    if (plantCorn(playerTilePos))
                    {
                        CCLOG("Successfully planted corn");
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

        // �������ˮ�����������׶�
        if (info.isWatered && info.growthStage < 3)  // ��������׶���4
        {
            int oldStage = info.growthStage;
            info.growthStage++;

            if (i < _crops.size() && _crops[i])
            {
                if (auto corn = dynamic_cast<Corn*>(_crops[i]))
                {
                    corn->updateGrowthStage(info.growthStage);
                }
            }
        }

        // ����ˮ��״̬
        int oldWaterLevel = info.waterLevel;
        info.waterLevel--;

        // ����ˮ��״̬��ʾ
        if (i < _crops.size() && _crops[i])
        {
            if (auto corn = dynamic_cast<Corn*>(_crops[i]))
            {
                corn->updateWaterStatus(info.waterLevel);
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
                info.growthStage = it->growthStage;
                info.plantDay = it->plantDay;
                info.plantMonth = it->plantMonth;
                info.plantYear = it->plantYear;
                info.waterLevel = it->waterLevel;
                info.isWatered = it->isWatered;
            }
            else
            {
                // ����������ʹ��Ĭ��ֵ
                info.growthStage = 0;
                auto gameTime = GameTime::getInstance();
                info.plantDay = gameTime->getDay();
                info.plantMonth = gameTime->getMonth();
                info.plantYear = gameTime->getYear();
                info.waterLevel = 2;
                info.isWatered = false;
            }

            info.type = "corn";
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
            // �����ջ������
            createHarvestDrop(_cropInfos[i].position);

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
        // ���õ�������ͼ���򣨵�һ�����һ�����ӣ�
        drop->setTextureRect(Rect(80, 0, 16, 16));

        // ����λ�ã���΢ƫ��һ�㣬������ȫ�ص���
        drop->setPosition(position + Vec2(30, 0));

        // �������ű���Ϊ2.0
        drop->setScale(2.0f);

        // ��ӵ�����
        _gameScene->addChild(drop);

        // ������ʱ����������
        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule([this, drop](float dt) {
            auto player = Player::getInstance();
            if (player)
            {
                // �������Ƿ��㹻����16�����ڣ�
                if (player->getPosition().distance(drop->getPosition()) < 16)
                {
                    // ��ӵ�����
                    ItemSystem::getInstance()->addItem("corn", 1);
                    // �Ƴ�������
                    drop->removeFromParent();
                    // ֹͣ��ʱ��
                    Director::getInstance()->getScheduler()->unschedule("check_drop_distance", drop);
                }
            }
            }, drop, 0.1f, false, "check_drop_distance");  // ÿ0.1����һ��
    }
}