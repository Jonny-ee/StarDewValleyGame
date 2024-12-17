#include "CropManager.h"

CropManager* CropManager::_instance = nullptr;  // 初始化静态成员变量

/*
 * 获取农作物管理系统单例实例
 * @return 农作物管理系统实例指针
 */
CropManager* CropManager::getInstance()
{
    if (!_instance)  // 如果实例不存在，创建实例
    {
        _instance = new CropManager();
    }
    return _instance;
}

/*
 * 检查指定位置是否可以开垦
 * @param tilePos 要检查的图片坐标
 * @return 如果可以开垦返回true，否则返回false
 */
bool CropManager::canTill(const Vec2& tilePos) const
{
    if (!_gameMap)  // 检查地图是否存在
        return false;

    if (_gameMap->getMapName() != "Farm")  // 只在在Farm地图开垦
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // 获取背景图层
    if (!backLayer)  // 检查图层是否存在
        return false;

    int tileGID = backLayer->getTileGIDAt(tilePos);  // 获取指定位置的图块ID

    return std::find(TILLABLE_TILES.begin(), TILLABLE_TILES.end(), tileGID) != TILLABLE_TILES.end();  // 检查是否是可开垦图块
}

/*
 * 开垦指定位置的土地
 * @param tilePos 要开垦的图片坐标
 * @return 开垦成功返回true，失败返回false
 */
bool CropManager::tillSoil(const Vec2& tilePos)
{
    if (!canTill(tilePos))  // 检查是否可以开垦
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // 获取背景图层
    if (!backLayer)  // 检查图层是否存在
        return false;

    backLayer->setTileGID(TILLED_TILE_ID, tilePos);  // 将图块设置为已开垦状态

    return true;
}

/*
 * 检查指定位置是否可以浇水
 */
bool CropManager::canWater(const Vec2& tilePos) const
{
    if (!_gameMap || _gameMap->getMapName() != "Farm")
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // 检查是否是已开垦的土地
    return backLayer->getTileGIDAt(tilePos) == TILLED_TILE_ID;
}

/*
 * 浇水指定位置的土地
 */
bool CropManager::waterSoil(const Vec2& tilePos)
{
    if (!canWater(tilePos))
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // 获取目标瓦片精灵
    Sprite* tile = backLayer->getTileAt(tilePos);
    if (!tile)
        return false;

    // 创建浇水效果
    createWaterEffect(tile);

    // 显示浇水提示
    showWateringPopup();

    return true;
}

/*
 * 创建浇水效果
 */
void CropManager::createWaterEffect(Sprite* tile)
{
    // 设置浇水后的颜色
    tile->setColor(WATER_COLOR);

    // 创建恢复原色的动画序列
    auto delay = DelayTime::create(WATER_DURATION);
    auto fadeBack = TintTo::create(FADE_DURATION, 255, 255, 255);
    tile->runAction(Sequence::create(delay, fadeBack, nullptr));
}

/*
 * 显示浇水提示
 */
void CropManager::showWateringPopup() {
    if (!_gameScene || !_gameMap) return;

    // 获取玩家位置
    auto player = Player::getInstance();
    if (!player) return;
    Vec2 playerPos = player->getPosition();

    // 创建半透明背景
    auto popupBg = LayerColor::create(Color4B(0, 0, 0, 150), 200, 80);

    // 创建文本标签
    auto label = Label::createWithSystemFont("Watering!", "Arial", 24);
    label->setPosition(Vec2(100, 40));
    label->setColor(Color3B::WHITE);
    popupBg->addChild(label);

    // 设置弹窗位置到玩家头顶
    popupBg->setPosition(Vec2(
        playerPos.x - 100,  // 居中显示
        playerPos.y + 50    // 在玩家上方
    ));

    // 添加到游戏场景
    _gameScene->addChild(popupBg, 1000);

    // 创建动画序列
    popupBg->setScale(0);
    popupBg->runAction(Sequence::create(
        ScaleTo::create(0.2f, 1.0f),
        DelayTime::create(1.0f),
        FadeOut::create(0.2f),
        RemoveSelf::create(),
        nullptr
    ));

    // 添加点击监听器
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
 * 处理鼠标按下事件，执行开垦操作
 * @param mousePos 鼠标位置
 * @param player 玩家对象指针
 */
void CropManager::onMouseDown(const Vec2& mousePos, Player* player)
{
    if (!player || !_gameMap)  // 检查必要对象
    {
        return;
    }

    if (_gameMap->getMapName() != "Farm")  // 确保只在Farm地图中执行
    {
        return;
    }

    Vec2 playerPos = player->getPosition();  // 获取玩家位置
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);  // 转换为瓦片坐标

    // 根据当前工具执行相应操作
    if (player->getCurrentTool() == Player::ToolType::SHOVEL)
    {
        if (canTill(playerTilePos))  // 检查是否可以开垦
        {
            if (tillSoil(playerTilePos))  // 尝试开垦土地
            {
                player->performAction(mousePos);  // 执行开垦动作
            }
        }
    }
    else if (player->getCurrentTool() == Player::ToolType::WATERING)
    {
        if (canWater(playerTilePos))  // 检查是否可以浇水
        {
            if (waterSoil(playerTilePos))  // 尝试浇水
            {
                player->performAction(mousePos);  // 执行浇水动作
            }
        }
    }
}

/*
 * 检查指定位置是否可以种植
 * @param tilePos 要检查的瓦片坐标
 * @return 如果可以种植返回true，否则返回false
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
 * 在指定位置种植玉米
 * @param tilePos 要种植的瓦片坐标
 * @return 种植成功返回true，否则返回false
 */
bool CropManager::plantCorn(const Vec2& tilePos)
{
    if (!canPlant(tilePos))
    {
        return false;
    }

    // 检查玩家背包中是否有玉米种子
    auto itemSystem = ItemSystem::getInstance();
    if (!itemSystem->hasEnoughItems("corn seed", 1))
    {
        CCLOG("No corn seeds available");
        return false;
    }

    // 扣除一个玉米种子
    if (!itemSystem->removeItem("corn seed", 1))
    {
        return false;
    }

    // 获取世界坐标并应用偏移
    Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
    worldPos.x += CROP_OFFSET_X;
    worldPos.y += CROP_OFFSET_Y;

    // 创建玉米作物
    auto corn = Corn::create(worldPos);
    if (corn)
    {
        _gameScene->addChild(corn, 0);
        _crops.push_back(corn);

        // 记录作物信息
        CropInfo info;
        info.position = worldPos;
        info.tilePos = tilePos;
        info.growthStage = 0;
        info.type = "corn";

        // 记录种植时间
        auto gameTime = GameTime::getInstance();
        info.plantDay = gameTime->getDay();
        info.plantMonth = gameTime->getMonth();
        info.plantYear = gameTime->getYear();

        _cropInfos.push_back(info);

        CCLOG("Corn planted on day %d, month %d, year %d",
            info.plantDay, info.plantMonth, info.plantYear);
        return true;
    }

    return false;
}

/*
 * 初始化键盘监听器
 * 监听P键用于种植操作
 */
void CropManager::initKeyboardListener()
{
    // 如果已经有监听器，直接返回
    if (_keyboardListener)
    {
        return;
    }

    _keyboardListener = cocos2d::EventListenerKeyboard::create();

    _keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
        {
            if (keyCode == EventKeyboard::KeyCode::KEY_P)
            {
                // 获取玩家当前位置
                auto player = Player::getInstance();
                if (!player || player->getCurrentTool() != Player::ToolType::NONE)
                {
                    return;
                }

                // 获取玩家所在的瓦片坐标
                Vec2 playerPos = player->getPosition();
                Vec2 tilePos = _gameMap->convertToTileCoord(playerPos);

                // 尝试种植玉米
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
 * 更新所有作物的生长状态
 * 每天调用一次
 */
void CropManager::updateCrops()
{
    // 即使不在农场地图，也要更新作物信息
    auto gameTime = GameTime::getInstance();
    int currentDay = gameTime->getDay();
    int currentMonth = gameTime->getMonth();
    int currentYear = gameTime->getYear();

    CCLOG("Updating crops: Current time %d/%d/%d, Total crops: %d",
        currentDay, currentMonth, currentYear, (int)_cropInfos.size());

    for (size_t i = 0; i < _cropInfos.size(); ++i)
    {
        auto& info = _cropInfos[i];

        // 计算经过的天数
        int daysElapsed = (currentYear - info.plantYear) * 365 +
            (currentMonth - info.plantMonth) * 30 +
            (currentDay - info.plantDay);

        // 计算新的生长阶段
        int newStage = daysElapsed;
        if (newStage > 4) newStage = 4;

        // 更新生长阶段
        if (newStage != info.growthStage)
        {
            CCLOG("Updating crop %zu from stage %d to %d",
                i, info.growthStage, newStage);
            info.growthStage = newStage;

            // 只有在农场地图时才更新显示
            if (_gameMap && _gameMap->getMapName() == "Farm" && i < _crops.size())
            {
                if (auto corn = dynamic_cast<Corn*>(_crops[i]))
                {
                    corn->updateGrowthStage(newStage);
                }
            }
        }
    }
}

/*
 * 保存当前地图上所有作物的信息
 */
void CropManager::saveCrops()
{
    std::vector<CropInfo> newCropInfos;  // 创建新的临时容器

    for (auto crop : _crops)
    {
        if (crop)
        {
            CropInfo info;
            info.position = crop->getPosition();
            info.tilePos = _gameMap->convertToTileCoord(crop->getPosition());

            // 在现有的 _cropInfos 中查找对应位置的作物信息
            auto it = std::find_if(_cropInfos.begin(), _cropInfos.end(),
                [&info](const CropInfo& existingInfo) {
                    return existingInfo.position == info.position;
                });

            if (it != _cropInfos.end())
            {
                // 如果找到了，使用现有的生长信息
                info.growthStage = it->growthStage;
                info.plantDay = it->plantDay;
                info.plantMonth = it->plantMonth;
                info.plantYear = it->plantYear;
            }
            else
            {
                // 如果是新作物，使用默认值
                info.growthStage = 0;
                auto gameTime = GameTime::getInstance();
                info.plantDay = gameTime->getDay();
                info.plantMonth = gameTime->getMonth();
                info.plantYear = gameTime->getYear();
            }

            info.type = "corn";
            newCropInfos.push_back(info);
        }
    }

    // 用新的信息替换旧的
    _cropInfos = std::move(newCropInfos);
}

/*
 * 加载并重新创建保存的作物
 */
void CropManager::loadCrops()
{
    // 只在Farm地图加载作物
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
                CCLOG("Loaded corn at (%.1f, %.1f) with growth stage %d", 
                      info.position.x, info.position.y, info.growthStage);
            }
        }
    }
}

/*
 * 清理当前地图上的所有作物精灵
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