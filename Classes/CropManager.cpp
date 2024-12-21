#include "CropManager.h"

// 初始化静态成员变量
CropManager* CropManager::_instance = nullptr;  // 实例

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
 * 检查指定位置是否可以移除资源
 * @param tilePos 要检查的图块坐标
 * @return 如果可以移除资源返回true，否则返回false
 */
bool CropManager::resourceCanRemove(const Vec2& tilePos) const
{
    if (!_gameMap)  // 检查地图是否存在
        return false;

    if (_gameMap->getMapName() != "Farm")  // 只在在Farm地图除草碎石
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // 获取resource图层
    if (!backLayer)  // 检查图层是否存在
        return false;

    int tileGID = backLayer->getTileGIDAt(tilePos);  // 获取指定位置的图块ID

    return std::find(RESOURCE_TILES.begin(), RESOURCE_TILES.end(), tileGID) != RESOURCE_TILES.end();  // 检查是否是可移除的资源图块
}


/*
 * 移除指定位置的资源
 * @param tilePos 要移除资源的图块坐标
 * @return 如果成功移除返回true，否则返回false
 */
bool CropManager::removeResource(const Vec2& tilePos)
{
    if (!resourceCanRemove(tilePos))  // 检查是否可以移除资源
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // 获取背景图层
    if (!backLayer)  // 检查图层是否存在
        return false;

    // 更新资源图层，将资源图块替换为已移除的图块
    backLayer->setTileGID(RESOURCE_REMOVED_TILE_ID, tilePos);

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
    // 更新作物的水分状态
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        if (_cropInfos[i].tilePos == tilePos)
        {
            CCLOG("Watering crop at position (%f, %f)", tilePos.x, tilePos.y);
            _cropInfos[i].isWatered = true;
            _cropInfos[i].waterLevel = 2;  // 恢复到充足状态
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
void CropManager::showWateringPopup()
{
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
    if (!player || !_gameMap)               // 检查必要对象
    {
        return;
    }

    if (_gameMap->getMapName() != "Farm")   // 确保只在Farm地图中执行
    {
        return;
    }

    Vec2 playerPos = player->getPosition();                         // 获取玩家位置
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);   // 转换为瓦片坐标

    // 根据当前工具执行相应操作
    if (player->getCurrentTool() == Player::ToolType::SHOVEL)
    {
        if (canTill(playerTilePos))                 // 检查是否可以开垦
        {
            if (tillSoil(playerTilePos))            // 尝试开垦土地
            {
                player->performAction(mousePos);    // 执行开垦动作
            }
        }
    }
    else if (player->getCurrentTool() == Player::ToolType::WATERING)
    {
        if (canWater(playerTilePos))                // 检查是否可以浇水
        {
            if (waterSoil(playerTilePos))           // 尝试浇水
            {
                player->performAction(mousePos);    // 执行浇水动作
            }
        }
    }
}

/*
 * 显示提示文本
 * @param text 要显示的文本
 * @param tilePos 瓦片坐标
 * @param duration 显示持续时间（0表示持续显示）
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

    // 设置位置在耕地上方
    if (_gameMap)
    {
        // 直接使用 GameMap 的坐标转换函数
        Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
        // 在世界坐标的基础上适当偏移
        tipLabel->setPosition(Vec2(worldPos.x + 20, worldPos.y + 40));
    }

    if (duration > 0)
    {
        tipLabel->stopAllActions();  // 停止之前的动作
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
 * 隐藏提示文本
 */
void CropManager::hideTip() const
{
    if (tipLabel)
    {
        tipLabel->setVisible(false);
    }
}

/*
 * 检查指定瓦片位置是否已有作物
 * @param tilePos 要检查的瓦片坐标
 * @return 如果该位置已有作物返回true，否则返回false
 */
bool CropManager::hasCropAt(const Vec2& tilePos) const
{
    // 检查该位置是否已有作物
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
 * 实时更新农作物相关提示
 * @param playerTilePos 玩家所在的瓦片坐标
 * @param playerTool 玩家当前工具
 */
void CropManager::updateTips(const Vec2& playerTilePos, Player::ToolType playerTool) const
{
    // 如果不在农场地图，不显示提示
    if (!_gameMap || _gameMap->getMapName() != "Farm")
    {
        hideTip();
        return;
    }
    // 检查该位置是否是耕地
    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
    {
        hideTip();
        return;
    }
    int tileGID = backLayer->getTileGIDAt(playerTilePos);

    // 空手且在耕地上
    if (playerTool == Player::ToolType::NONE && tileGID == TILLED_TILE_ID && !hasCropAt(playerTilePos))
    {
        showTip("Press P to plant", playerTilePos);
        return;
    }

    // 装备斧头且在有作物的地块上
    if (playerTool == Player::ToolType::AXE && hasCropAt(playerTilePos))
    {
        // 检查作物是否成熟
        for (const auto& crop : _cropInfos)
        {
            if (crop.tilePos == playerTilePos && crop.growthStage == FINAL_GROWTH_STAGE)
            {
                showTip("Press H to harvest", playerTilePos);
                return;
            }
        }
    }
    // 检查是否可以施肥
    if (canFertilize(playerTilePos) &&
        ItemSystem::getInstance()->getItemCount("fertilizer") > 0)
    {
        showTip("Press F to fertilize", playerTilePos);
        return;
    }
    // 其他情况隐藏提示（除了当前位置已有作物，即可能重复种植）
    if (!hasCropAt(playerTilePos))
    {
        hideTip();
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
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // 检查是否已有作物
    if (hasCropAt(tilePos))
    {
        showTip("Cannot plant here again!", tilePos);
        return false;
    }

    // 检查是否是已耕地或已浇水的耕地
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
        info.waterLevel = 1;        // 初始视为较为缺水（需要浇水第二天才会长）
        info.isWatered = false;     // 种植时视为未浇水

        _cropInfos.push_back(info);

        CCLOG("Corn planted on day %d, month %d, year %d",
            info.plantDay, info.plantMonth, info.plantYear);
        return true;
    }

    return false;
}

/*
 * 在指定位置种植番茄
 * @param tilePos 要种植的瓦片坐标
 * @return 种植成功返回true，否则返回false
 */
bool CropManager::plantTomato(const Vec2& tilePos)
{
    if (!canPlant(tilePos))
    {
        return false;
    }
    // 检查玩家背包中是否有番茄种子
    auto itemSystem = ItemSystem::getInstance();
    if (!itemSystem->hasEnoughItems("tomato seed", 1))
    {
        CCLOG("No tomato seeds available");
        return false;
    }
    // 扣除一个番茄种子
    if (!itemSystem->removeItem("tomato seed", 1))
    {
        return false;
    }
    // 获取世界坐标并应用偏移
    Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
    worldPos.x += CROP_OFFSET_X;
    worldPos.y += CROP_OFFSET_Y;
    // 创建番茄作物
    auto tomato = Tomato::create(worldPos);
    if (tomato)
    {
        _gameScene->addChild(tomato, 0);
        _crops.push_back(tomato);
        // 记录作物信息
        CropInfo info;
        info.position = worldPos;
        info.tilePos = tilePos;
        info.growthStage = 0;
        info.type = "tomato";
        // 记录种植时间
        auto gameTime = GameTime::getInstance();
        info.plantDay = gameTime->getDay();
        info.plantMonth = gameTime->getMonth();
        info.plantYear = gameTime->getYear();
        info.waterLevel = 1;        // 初始视为较为缺水
        info.isWatered = false;     // 种植时视为未浇水
        info.growthCounter = 0;     // 成长计数器初始化为0
        _cropInfos.push_back(info);
        CCLOG("Tomato planted on day %d, month %d, year %d",
            info.plantDay, info.plantMonth, info.plantYear);
        return true;
    }
    return false;
}

/*
 * 根据当前的种子类型判断种植作物
 * @param tilePos 要种植的瓦片坐标
 * @return 装备有效种子返回true，否则返回false
 */
bool CropManager::plantCrop(const Vec2& tilePos)
{
    auto player = Player::getInstance();
    if (!player)
        return false;
    // 根据当前选择的种子类型来种植
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
 * 初始化键盘监听器
 * 监听P键用于种植操作
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

            // 获取玩家位置
            Vec2 playerPos = player->getPosition();
            Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);
            if (keyCode == EventKeyboard::KeyCode::KEY_P)  // P键种植
            {
                if (player->getCurrentTool() == Player::ToolType::NONE)
                {
                    if (plantCrop(playerTilePos))
                    {
                        CCLOG("Successfully planted crop");
                    }
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_H)  // H键收获
            {
                if (player->getCurrentTool() == Player::ToolType::AXE)  // 需要装备斧头
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
            else if (keyCode == EventKeyboard::KeyCode::KEY_F)  // F键施肥
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
 * 更新所有作物的生长状态
 * 每天调用一次
 */
void CropManager::updateCrops()
{
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        auto& info = _cropInfos[i];
        // 先检查水分状态
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
        // 根据作物类型判断是否可以生长
        // 玉米的生长逻辑
        if (info.type == "corn")
        {
            // 玉米只需要浇水就能生长
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
        // 番茄的生长逻辑
        else if (info.type == "tomato")
        {
            // 确保 growthCounter 不会出现负数
            if (info.growthCounter > 1000000) {  // 如果出现异常大的数字
                info.growthCounter = 0;  // 重置为0
            }
            if (info.isWatered)  // 今天浇了水
            {
                info.growthCounter++;
                CCLOG("Tomato at position (%.1f, %.1f) - Growth counter increased to: %u",
                    info.position.x, info.position.y, info.growthCounter);
                // 只有当计数器达到2时才生长
                if (info.growthCounter == 2 &&
                    info.growthStage < Tomato::getTotalGrowthStages() - 1)
                {
                    info.growthStage++;
                    info.growthCounter = 0;  // 生长后重置计数器
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
            else if (info.waterLevel < 2)  // 只有当水分不足时才重置计数器
            {
                info.growthCounter = 0;
                CCLOG("Tomato growth counter reset due to insufficient water level");
            }
        }

        // 降低水分状态
        info.waterLevel--;
        // 更新水分状态显示
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
        // 重置浇水状态
        info.isWatered = false;
    }
}

/*
 * 保存当前地图上所有作物的信息
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

            // 在现有的 _cropInfos 中查找对应位置的作物信息
            auto it = std::find_if(_cropInfos.begin(), _cropInfos.end(),
                [&info](const CropInfo& existingInfo) {
                    return existingInfo.position == info.position;
                });
            if (it != _cropInfos.end())
            {
                // 如果找到了，使用现有的所有信息
                info = *it;  // 复制所有信息，包括 growthCounter
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
                info.growthCounter = 0;  // 初始化计数器

                // 根据作物类型设置
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

/*
 * 检查指定位置是否可以收获
 * @param tilePos 要检查的瓦片坐标
 * @return 如果作物已成熟可以收获返回true，否则返回false
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
 * 收获指定位置的作物
 * @param tilePos 要收获的瓦片坐标
 * @return 收获成功返回true，否则返回false
 */
bool CropManager::harvestCrop(const Vec2& tilePos)
{
    for (size_t i = 0; i < _cropInfos.size(); i++)
    {
        if (_cropInfos[i].tilePos == tilePos && _cropInfos[i].growthStage == FINAL_GROWTH_STAGE)
        {
            // 创建收获掉落
            createHarvestDrop(_cropInfos[i].position);

            // 获取当前种植等级
            auto skillSystem = SkillSystem::getInstance();
            int currentLevel = skillSystem->getSkillLevel(SkillType::FARMING);

            // 增加种植经验
            skillSystem->gainExp(SkillType::FARMING, 10);

            // 检查是否刚刚升到3级
            if (currentLevel < 3 && skillSystem->getSkillLevel(SkillType::FARMING) >= 3)
            {
                // 达到3级后给予番茄种子
                auto itemSystem = ItemSystem::getInstance();
                itemSystem->addItem("tomato seed", 1);
                CCLOG("Unlocked tomato seed at farming level 3");
            }

            // 移除作物
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
 * 创建收获后的掉落物
 * @param position 掉落物生成的世界坐标位置
 * 功能：
 * 1.创建掉落物精灵
 * 2.设置掉落物外观和位置
 * 3.添加自动拾取检测
 */
void CropManager::createHarvestDrop(const Vec2& position)
{
    // 创建掉落物精灵
    auto drop = Sprite::create("Plants.png");
    if (drop)
    {
        // 根据作物类型设置掉落物贴图区域
        for (const auto& info : _cropInfos)
        {
            if (info.position == position)
            {
                if (info.type == "corn")
                {
                    drop->setTextureRect(Rect(80, 0, 16, 16));      // 玉米收获物
                }
                else if (info.type == "tomato")
                {
                    drop->setTextureRect(Rect(80, 16, 16, 16));     // 番茄收获物
                }
                break;
            }
        }
        // 设置位置（稍微偏移一点，避免完全重叠）
        drop->setPosition(position + Vec2(30, 0));
        // 设置缩放比例为2.0
        drop->setScale(2.0f);
        // 添加到场景
        _gameScene->addChild(drop);
        // 创建定时器来检查距离
        auto scheduler = Director::getInstance()->getScheduler();
        scheduler->schedule([this, drop, position](float dt) {
            auto player = Player::getInstance();
            if (player)
            {
                // 检查玩家是否足够近（16像素内）
                if (player->getPosition().distance(drop->getPosition()) < 16)
                {
                    // 根据作物类型添加对应的收获物
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
                    // 移除掉落物
                    drop->removeFromParent();
                    // 停止定时器
                    Director::getInstance()->getScheduler()->unschedule("check_drop_distance", drop);
                }
            }
            }, drop, 0.1f, false, "check_drop_distance");  // 每0.1秒检查一次
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
            // 消耗肥料
            itemSystem->removeItem("fertilizer", 1);

            // 增加生长阶段
            _cropInfos[i].growthStage++;

            // 更新作物显示
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