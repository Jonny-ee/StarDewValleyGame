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
void CropManager::showWateringPopup() {
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
    if (!player || !_gameMap)  // ����Ҫ����
    {
        return;
    }

    if (_gameMap->getMapName() != "Farm")  // ȷ��ֻ��Farm��ͼ��ִ��
    {
        return;
    }

    Vec2 playerPos = player->getPosition();  // ��ȡ���λ��
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);  // ת��Ϊ��Ƭ����

    // ���ݵ�ǰ����ִ����Ӧ����
    if (player->getCurrentTool() == Player::ToolType::SHOVEL)
    {
        if (canTill(playerTilePos))  // ����Ƿ���Կ���
        {
            if (tillSoil(playerTilePos))  // ���Կ�������
            {
                player->performAction(mousePos);  // ִ�п��Ѷ���
            }
        }
    }
    else if (player->getCurrentTool() == Player::ToolType::WATERING)
    {
        if (canWater(playerTilePos))  // ����Ƿ���Խ�ˮ
        {
            if (waterSoil(playerTilePos))  // ���Խ�ˮ
            {
                player->performAction(mousePos);  // ִ�н�ˮ����
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
        _crops.push_back(corn);     // ��ӵ�����
        CCLOG("Corn added to scene with adjusted position: (%.1f, %.1f)", 
              worldPos.x, worldPos.y);
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
    // ����Ѿ��м�������ֱ�ӷ���
    if (_keyboardListener)
    {
        return;
    }

    _keyboardListener = cocos2d::EventListenerKeyboard::create();

    _keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
        {
            if (keyCode == EventKeyboard::KeyCode::KEY_P)
            {
                // ��ȡ��ҵ�ǰλ��
                auto player = Player::getInstance();
                if (!player || player->getCurrentTool() != Player::ToolType::NONE)
                {
                    return;
                }

                // ��ȡ������ڵ���Ƭ����
                Vec2 playerPos = player->getPosition();
                Vec2 tilePos = _gameMap->convertToTileCoord(playerPos);

                // ������ֲ����
                if (plantCorn(tilePos))
                {
                    CCLOG("Successfully planted corn at tile position (%.1f, %.1f)",
                        tilePos.x, tilePos.y);
                }
            }
        };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        _keyboardListener, _gameMap->getTileMap());
}

/*
 * ���浱ǰ��ͼ�������������Ϣ
 */
void CropManager::saveCrops()
{
    _cropInfos.clear();

    // ����ÿ���������Ϣ
    for (auto crop : _crops)
    {
        if (crop)
        {
            CropInfo info;
            info.position = crop->getPosition();
            info.tilePos = _gameMap->convertToTileCoord(crop->getPosition());
            info.growthStage = 0;  // Ŀǰ�ȴ�0������ʵ������ϵͳ�����޸�
            info.type = "corn";    // Ŀǰֻ�����ף�������Ӹ�����������ʱ���޸�

            _cropInfos.push_back(info);
        }
    }
}

/*
 * ���ز����´������������
 */
void CropManager::loadCrops()
{
    clearCrops();  // ������ǰ������

    // ���´������б��������
    for (const auto& info : _cropInfos)
    {
        if (info.type == "corn")
        {
            auto corn = Corn::create(info.position);
            if (corn)
            {
                _gameScene->addChild(corn, 0);
                _crops.push_back(corn);
                // ������������׶ε�����
                // corn->updateGrowthStage(info.growthStage);
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