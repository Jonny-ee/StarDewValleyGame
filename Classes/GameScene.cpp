#include "GameScene.h"
#include "GameTime.h"
#include "LightManager.h"
#include"WeatherManager.h"
#include "Chest.h" 
#include "Sleep.h"
#include "BridgeEvent.h"
#include "Cooking.h"
#include "CropManager.h"
#include "NormalWeather.h"
#include"AudioManager.h"
USING_NS_CC;

Scene* GameScene::createScene()
{

    return GameScene::create();
}

void GameScene::initToolIcon()
{
    toolIcon = Sprite::create("tools.png");
    if (toolIcon)
    {
        toolIcon->setScale(3.0f);
        this->addChild(toolIcon, 10);
        toolIcon->setPosition(Player::TOOL_ICON_POSITION);
        updateToolIcon();
    }
}

void GameScene::updateToolIcon()
{
    if (!toolIcon || !player) return;

    // 根据玩家当前工具设置图标
    int toolIndex = static_cast<int>(player->getCurrentTool());

    // 根据实际的枚举值设置对应的纹理区域
    switch (toolIndex) {
        case 0:  // NONE
            toolIcon->setVisible(false);        // 没有工具，隐藏贴图
            break;
        case 1:  // SHOVEL
            toolIcon->setTexture("tools.png");  // 使用原来的工具贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(32, 0, 16, 16));
            break;
        case 2:  // AXE
            toolIcon->setTexture("tools.png");  // 使用原来的工具贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(16, 0, 16, 16));
            break;
        case 3:  // PICK
            toolIcon->setTexture("TileSheets/tools.png");  // 使用新的工具贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(80, 96, 16, 16));
            break;
        case 4:  // WATERING
            toolIcon->setTexture("tools.png");  // 使用原来的工具贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(0, 0, 16, 16));
            break;
        case 5:  // ROD
            toolIcon->setTexture("TileSheets/Tools.png");  // 使用新的贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(128, 0, 16, 16));
            break;
        case 6:  // GIFT
            toolIcon->setTexture("TileSheets/Objects_2.png");  // 使用新的贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(96, 32, 16, 16));
            break;
        case 7:  // CARROT
            toolIcon->setTexture("LooseSprites/emojis.png");  // 使用新的贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(18, 36, 9, 9));
            break;
    }
}

void GameScene::initSeedIcon()
{
    seedIcon = Sprite::create("Plants.png");
    if (seedIcon)
    {
        seedIcon->setScale(3.0f);
        this->addChild(seedIcon, 10);
        seedIcon->setPosition(Player::SEED_ICON_POSITION);

        // 初始化时根据当前地图设置可见性
        seedIcon->setVisible(_gameMap && _gameMap->getMapName() == "Farm");

        updateSeedIcon();
    }
}

void GameScene::updateSeedIcon()
{
    if (!seedIcon || !player)
        return;

    // 只在农场地图显示种子图标
    if (_gameMap)
    {
        seedIcon->setVisible(_gameMap->getMapName() == "Farm");
    }
    // 根据当前选择的种子类型设置贴图区域
    switch (player->getCurrentSeed())
    {
        case Player::SeedType::CORN:
            seedIcon->setTextureRect(Rect(0, 0, 16, 16));  // 第一行第一个
            break;
        case Player::SeedType::TOMATO:
            seedIcon->setTextureRect(Rect(0, 16, 16, 16)); // 第二行第一个
            break;
    }
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // 初始化变量
    player = nullptr;
    _gameMap = nullptr;
    _pressedKeys.clear();
    isLewisCreated = false;     // 初始化标志
    isMarlonCreated = false;    // 初始化标志
    isMaruCreated = false;      // 初始化标志
    isAlexCreated = false;      // 初始化标志
    isMuted = false; // 初始化为未静音

    // 初始化矿洞进入时间记录
    lastMineEnterDay = 0;
    lastMineEnterMonth = 0;
    lastMineEnterYear = 0;

    // 创建并加载地图
    _gameMap = GameMap::create("First");
    if (_gameMap == nullptr)
    {
        return false;
    }
    this->addChild(_gameMap);

    // 创建玩家
    player = Player::create();
    if (player == nullptr)
    {
        return false;
    }

    // 设置玩家初始位置
    const Vec2 tilePos = Vec2(14.5, 15);
    const Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
    player->setPosition(worldPos);
    player->setScale(3.0f);

    this->addChild(player, 1);

    // 初始化钓鱼区域
    FishingSystem::getInstance()->initFishingAreas(_gameMap);

    // 初始化鼠标监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [this](Event* event)
        {
            auto fishingSystem = FishingSystem::getInstance();

            // 如果已经在钓鱼中,则尝试完成钓鱼
            if (fishingSystem->isCurrentlyFishing())
            {
                fishingSystem->finishFishing();
                return;
            }

            // 如果还没开始钓鱼,检查是否可以开始钓鱼
            if (fishingSystem->canFish(player->getPosition(), player))
            {
                fishingSystem->startFishing();
            }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // 设置关联和事件监听
    player->setGameMap(_gameMap);
    player->removeAllListeners(); // 移除可能存在的旧监听器
    player->initKeyboardListener();
    player->initMouseListener();

    // 创建背包UI
    _inventoryUI = InventoryUI::create();
    if (_inventoryUI)
    {
        this->addChild(_inventoryUI, 10);
        _inventoryUI->setVisible(false);

        // 设置物品系统的更新回调
        ItemSystem::getInstance()->setUpdateCallback([this]() {
            if (_inventoryUI)
            {
                _inventoryUI->updateDisplay();
            }
            });
    }

    // 启动更新
    this->scheduleUpdate();

    // 初始化工具图标
    initToolIcon();

    // 初始化种子图标
    initSeedIcon();

    // 初始化鼠标监听器
    initMouseListener();

    // 初始化宝箱
    initChests();

    // 设置CropManager的地图引用
    auto cropManager = CropManager::getInstance();
    cropManager->setGameScene(this);
    CropManager::getInstance()->setGameMap(_gameMap);

    // 创建状态UI
    _statusUI = StatusUI::create();
    if (_statusUI) {
        // 设置位置到右上角
        const Size visibleSize = Director::getInstance()->getVisibleSize();
        const Vec2 origin = Director::getInstance()->getVisibleOrigin();
        _statusUI->setPosition(origin.x + visibleSize.width,
            origin.y + visibleSize.height);

        // 添加到最上层，确保不会被其他内容遮挡
        this->addChild(_statusUI, 10);
    }

    // 创建事件
    _events.push_back(SleepEvent::create(_gameMap, player));
    _events.push_back(BridgeEvent::create(_gameMap, player));
    _events.push_back(Cooking::create(_gameMap, player));
    for (auto event : _events) {
        this->addChild(event);
    }

    ItemSystem* itemSystem = ItemSystem::getInstance();
    itemSystem->addItem("corn seed", 5);
    itemSystem->addItem("tomato seed", 5);
    itemSystem->addItem("fertilizer", 5);

    auto weatherManager = WeatherManager::getInstance();
    weatherManager->setWeather(NormalWeather::create());

    AudioManager::getInstance()->playBGM("normal.mp3");

    //// 创建静音按钮
    //muteButton = MenuItemImage::create(
    //    "mute.png", // 静音图标
    //    "unmute.png", // 取消静音图标
    //    CC_CALLBACK_1(GameScene::toggleMute, this) // 点击事件回调
    //);

    //// 设置按钮位置
    //muteButton->setPosition(Vec2(50, Director::getInstance()->getVisibleSize().height - 50));

    //// 创建菜单并添加按钮
    //auto menu = Menu::create(muteButton, nullptr);
    //menu->setPosition(Vec2::ZERO); // 菜单位置
    //this->addChild(menu, 10); // 添加到场景中

    return true;
}
// 切换静音状态
void GameScene::toggleMute(Ref* sender) {
    isMuted = !isMuted; // 切换静音状态
    if (isMuted) {
        AudioManager::getInstance()->setVolume(0.0f); // 设置音量为0
        muteButton->setNormalImage(Sprite::create("unmute.png")); // 更改按钮图标
    }
    else {
        AudioManager::getInstance()->setVolume(1.0f); // 恢复音量
        muteButton->setNormalImage(Sprite::create("mute.png")); // 更改按钮图标
    }
}
void GameScene::onDayChanged()
{
    // 更新作物生长
    CropManager::getInstance()->updateCrops();
    auto weatherManager = WeatherManager::getInstance();
    weatherManager->randomRefreshWeather();
    // 刷新资源
    _gameMap->refreshResources();

    // 重置任务状态
    auto questSystem = QuestSystem::getInstance();
    auto gameTime = GameTime::getInstance();
    int newDay = gameTime->getDay();

    // 第一天开始时，重置木头收集任务
    if (newDay == 1) {
        questSystem->resetQuest(QuestType::COLLECT_WOOD);
        if (lewis) {
            showQuestMark(lewis);
        }
    }
    // 第二天开始时，重置修桥任务
    else if (newDay == 2) {
        // 重置修桥任务
        questSystem->resetQuest(QuestType::REPAIR_BRIDGE);
        if (lewis) {
            showQuestMark(lewis);
        }
    }

    // 移除任务提示UI
    if (_questTipLabel) {
        _questTipLabel->removeFromParent();
        _questTipLabel = nullptr;
    }

    // 这里可以添加其他每日更新的内容
    CCLOG("A new day has started!");
}

void GameScene::update(float dt)
{
    if (!player || !_gameMap)
    {
        return;
    }

    // 获取GameTime单例实例
    GameTime* gameTime = GameTime::getInstance();

    // 记录更新前的日期
    int oldDay = gameTime->getDay();

    // 更新游戏时间
    gameTime->update();

    // 检查是否日期发生变化
    if (gameTime->getDay() != oldDay)
    {
        onDayChanged();
    }

    // 更新光照效果
    LightManager::getInstance()->update();

    // 只保留一次update调用
    player->update(dt);

    // 检查传送点
    Vec2 playerTilePos = _gameMap->convertToTileCoord(player->getPosition());
    TransitionInfo transition;
    if (_gameMap->checkForTransition(playerTilePos, transition))
    {
        switchToMap(transition.targetMap, transition.targetTilePos);
    }

    updateToolIcon();   // 每帧更新工具图标
    updateSeedIcon();   // 每帧更新种子图标

    // 更新Lewis的状态
    if (lewis)
    {
        lewis->moveAlongPath(dt); // 移动沿路径
    }

    // 更新所有猪的状态
    for (auto pig : pigs)
    {
        if (pig)
        {
            pig->moveAlongPath(dt); // 移动沿路径
        }
    }
    // 更新所有鸡的状态
    for (auto chicken : chickens)
    {
        if (chicken)
        {
            chicken->moveAlongPath(dt); // 移动沿路径
        }
    }
    // 更新所有羊的状态
    for (auto sheep : sheeps)
    {
        if (sheep)
        {
            sheep->moveAlongPath(dt); // 移动沿路径
        }
    }
    // 持续检查钓鱼条件
    auto fishingSystem = FishingSystem::getInstance();
    fishingSystem->canFish(player->getPosition(), player);
    // 更新提示标签位置
    if (auto tipLabel = fishingSystem->getTipLabel())
    {
        if (tipLabel->isVisible())
        {
            Vec2 playerPos = player->getPosition();
            // 设置在玩家头顶上方50像素
            tipLabel->setPosition(playerPos + Vec2(0, 50));
        }
    }

    // 更新作物提示
    CropManager::getInstance()->updateTips(playerTilePos, player->getCurrentTool());
    // 更新杀虫状态
    CropManager::getInstance()->updateBugKilling(dt);
    // 检查所有事件
    for (auto event : _events)
    {
        event->update(dt);
    }

    // 更新前景瓦片可见性
    if (_gameMap && player)
    {
        _gameMap->updateFrontTileVisibility(player->getPosition());
    }

    updateCamera();
    checkQuestProgress();  // 检查任务进度
}

void GameScene::updateCamera()
{
    if (!player || !_gameMap)
        return;
    if (_gameMap->getMapName() == "First")
        return;

    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Size mapSize = _gameMap->getTileMap()->getMapSize();
    const Size tileSize = _gameMap->getTileMap()->getTileSize();
    const float scale = _gameMap->getTileMap()->getScale();

    // 计算地图的实际像素大小
    const  float mapWidth = mapSize.width * tileSize.width * scale;
    const float mapHeight = mapSize.height * tileSize.height * scale;

    // 获取玩家位置
    const Vec2 playerPos = player->getPosition();

    float x, y;

    // 如果地图小于屏幕，则居中显示
    if (mapWidth < visibleSize.width)
    {
        x = visibleSize.width / 2;
    }
    else
    {
        x = std::max(playerPos.x, visibleSize.width / 2);
        x = std::min(x, mapWidth - visibleSize.width / 2);
    }

    if (mapHeight < visibleSize.height)
    {
        y = visibleSize.height / 2;
    }
    else
    {
        y = std::max(playerPos.y, visibleSize.height / 2);
        y = std::min(y, mapHeight - visibleSize.height / 2);
    }

    const Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    const Vec2 pointB = Vec2(x, y);
    Vec2 offset = pointA - pointB;


    // 如果地图小于屏幕，调整偏移量使地图居中
    if (mapWidth < visibleSize.width)
    {
        offset.x = (visibleSize.width - mapWidth) / 2;
    }
    if (mapHeight < visibleSize.height)
    {
        offset.y = (visibleSize.height - mapHeight) / 2;
    }

    this->setPosition(offset);

    // 确保背包UI也跟随相机
    if (_inventoryUI)
    {
        _inventoryUI->setPosition(-offset);
    }
    // 更新工具图标位置，使其保持在视图左下角
    if (toolIcon)
    {
        toolIcon->setPosition(-offset + Vec2(50, 50));  // 左下角偏移50像素
    }
    if (seedIcon)
    {
        seedIcon->setPosition(-offset + Vec2(100, 50)); // 工具图标右侧50像素
    }
    // 更新对话框位置
    if (dialogueBox)
    {
        dialogueBox->setPosition(-offset + Vec2(visibleSize.width / 2, 90));
    }

    // 更新UI位置，确保状态UI始终固定在右上角
    if (_statusUI) {
        _statusUI->setPosition(-offset + Vec2(visibleSize.width, visibleSize.height));
    }

    // 更新任务UI位置
    updateQuestUIPosition();
}

void GameScene::switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos)
{
    // 如果当前是农场地图，保存作物信息
    if (_gameMap && _gameMap->getMapName() == "Farm")
    {
        CropManager::getInstance()->saveCrops();
        CropManager::getInstance()->clearCrops();  // 离开农场时清理显示
    }
    // 移除任务UI（在移除其他UI之前）
    if (_questTipLabel) {
        _questTipLabel->removeFromParent();
        _questTipLabel = nullptr;
    }
    if (_questMark) {
        _questMark->removeFromParent();
        _questMark = nullptr;
    }

    // 清理宝箱
    clearChests();

    // 移除当前 NPC
    if (lewis) {
        lewis->removeFromParent(); // 移除刘易斯
        lewis = nullptr; // 清空指针
    }
    if (marlon) {
        marlon->removeFromParent(); // 移除马龙
        marlon = nullptr; // 清空指针
    }
    if (maru) {
        maru->removeFromParent(); // 移除玛鲁
        maru = nullptr; // 清空指针
    }
    if (alex) {
        alex->removeFromParent(); // 移除艾利克斯
        alex = nullptr; // 清空指针
    }
    // 移除所有猪
    for (auto pig : pigs) {
        if (pig) {
            pig->removeFromParent(); // 移除猪
        }
    }
    pigs.clear(); // 清空猪的向量
    // 移除所有鸡
    for (auto chicken : chickens) {
        if (chicken) {
            chicken->removeFromParent(); // 移除鸡
        }
    }
    chickens.clear(); // 清空鸡的向量
    // 移除所有羊
    for (auto sheep : sheeps) {
        if (sheep) {
            sheep->removeFromParent(); // 移除羊
        }
    }
    sheeps.clear(); // 清空羊的向量
    // 移除所有树
    for (auto tree : trees) {
        if (tree) {
            tree->setVisible(false); // 设置为不可见
        }
    }
    // 移除所有矿石
    for (auto ore : ores) {
        if (ore) {
            ore->setVisible(false); // 设置为不可见
        }
    }

    // 保存当前背包UI的引用和状态
    auto currentInventoryUI = _inventoryUI;
    bool wasInventoryVisible = false;
    if (currentInventoryUI) {
        wasInventoryVisible = currentInventoryUI->isVisible();
        currentInventoryUI->retain();
        currentInventoryUI->removeFromParent();
    }

    // 保存当前玩家的引用
    auto currentPlayer = player;
    if (currentPlayer) {
        currentPlayer->retain();
        currentPlayer->removeFromParent();
    }

    // 移除旧地图的显示
    if (_gameMap) {
        _gameMap->getTileMap()->removeFromParent();  // 从显示层级中移除旧地图
    }
    auto _gameTime = GameTime::getInstance();
    auto currentMonth = _gameTime->getMonth();
    auto currentDay = _gameTime->getDay();
    // 检查是否是切换到Town地图
    if (mapName == "Town") {
        // 如果是12月25日，传送到Town_Christmas
        if (currentMonth == 3 && currentDay == 3) {
            AudioManager::getInstance()->pauseBGM();
            AudioManager::getInstance()->playBGM("Christmas.mp3");
            CCLOG("Switching to Town_Christmas map");
            try { _gameMap->loadMap("Town_Christmas"); }
            catch (const std::exception& e)
            {
                CCLOG("wrong Path");
                _gameMap->loadMap("Town");
                throw;
            }
        }
        else {
            AudioManager::getInstance()->pauseBGM();
            AudioManager::getInstance()->playBGM("normal.mp3");
            // 否则传送到普通的Town地图
            CCLOG("Switching to Town map");
            _gameMap->loadMap("Town");
        }
    }
    else {
        AudioManager::getInstance()->pauseBGM();
        AudioManager::getInstance()->playBGM("normal.mp3");
        // 其他地图的处理逻辑
        _gameMap->loadMap(mapName);
    }

    // 重用现有玩家，而不是创建新的（修复原来多重玩家的bug）
    if (currentPlayer) {
        const Vec2 worldPos = _gameMap->convertToWorldCoord(targetTilePos);
        currentPlayer->setPosition(worldPos);
        currentPlayer->setGameMap(_gameMap);
        this->addChild(currentPlayer, 1);
        currentPlayer->release();
    }

    // 重新添加背包UI
    if (currentInventoryUI) {
        this->addChild(currentInventoryUI, 10);
        currentInventoryUI->setVisible(wasInventoryVisible);
        currentInventoryUI->release();
    }

    // 更新CropManager的地图引用
    CropManager::getInstance()->setGameMap(_gameMap);

    // 如果切换到农场地图，加载作物，显示当前种子，初始化刘易斯和动物
    if (mapName == "Farm")
    {
        if (seedIcon)
        {
            seedIcon->setVisible(true);
        }
        CropManager::getInstance()->loadCrops();
        CCLOG("Switching to Farm map, loading crops...");
        initLewis();  // initLewis会根据任务状态重新创建任务标记
        initPig();
        initChicken();
        initSheep();
        initTree();
        for (auto tree : trees) {
            if (tree) {
                tree->setVisible(true);
            }
        }

        // 如果有进行中的任务，重新创建任务UI
        const auto questSystem = QuestSystem::getInstance();

        if ((currentDay == 1 && questSystem->getQuestState(QuestType::COLLECT_WOOD) == QuestState::IN_PROGRESS) ||
            (currentDay == 2 && questSystem->getQuestState(QuestType::REPAIR_BRIDGE) == QuestState::IN_PROGRESS)) {
            if (!_questTipLabel) {
                _questTipLabel = Label::createWithTTF("", "fonts/arial.ttf", 24);
                if (_questTipLabel) {
                    this->addChild(_questTipLabel, 10);
                    _questTipLabel->setAnchorPoint(Vec2(1, 1));
                    updateQuestUIPosition();
                    updateQuestUI();
                }
            }
        }
    }
    // 在其他地图隐藏种子图标
    else
    {
        if (seedIcon)
        {
            seedIcon->setVisible(false);
        }
    }

    // 如果是医院地图，初始化玛鲁
    if (mapName == "Hospital") {
        CCLOG("Switching to Hospital map, initializing maru...");
        initMaru();
    }

    // 如果是小镇地图，初始化艾利克斯
    if (mapName == "Town") {
        CCLOG("Switching to Town map, initializing alex...");
        initAlex();
    }

    // 如果是矿洞地图，检查是否需要刷新宝箱，初始化矿石
    if (mapName == "Mine") {
        CCLOG("Switch to the mine map...");

        const  int currentYear = _gameTime->getYear();

        // 检查是否需要刷新宝箱
        bool shouldRefreshChests = false;

        // 如果是第一次进入矿洞
        if (lastMineEnterDay == 0) {
            shouldRefreshChests = true;
        }
        // 或者已经过了至少一天
        else if (currentYear > lastMineEnterYear ||
            (currentYear == lastMineEnterYear && currentMonth > lastMineEnterMonth) ||
            (currentYear == lastMineEnterYear && currentMonth == lastMineEnterMonth && currentDay > lastMineEnterDay)) {
            shouldRefreshChests = true;
        }

        // 如果需要刷新宝箱
        if (shouldRefreshChests) {
            CCLOG("Refresh the mine treasure chest...");
            initChests();
            // 更新进入时间
            lastMineEnterDay = currentDay;
            lastMineEnterMonth = currentMonth;
            lastMineEnterYear = currentYear;
        }
        else {
            CCLOG("The treasure chest has been refreshed today and will not be refreshed again");
        }

        // 初始化马龙NPC
        initMarlon();

        initOre();
        for (auto ore : ores) {
            if (ore) {
                ore->setVisible(true);
            }
        }
    }

    // 重新初始化钓鱼系统
    FishingSystem::getInstance()->initFishingAreas(_gameMap);
}

void GameScene::initMouseListener()
{
    auto mouseListener = EventListenerMouse::create();

    // 设置默认光标
    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");

    mouseListener->onMouseMove = [=](Event* event)
        {
            EventMouse* e = (EventMouse*)event;
            const Vec2 mousePos = e->getLocation();

            if (lewis) {
                const float distance = player->getPosition().distance(lewis->getPosition());

                if (distance < 50.0f) {
                    // 鼠标靠近Lewis，手上有礼物，变成礼物光标
                    if (player->getCurrentTool() == Player::ToolType::GIFT) {
                        Director::getInstance()->getOpenGLView()->setCursor("cursor_gift.png");
                    }
                    // 鼠标靠近Lewis，变成交互光标
                    else {
                        Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                    }
                }
                else {
                    // 鼠标远离Lewis，恢复默认光标
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
            if (marlon) {
                const float distance = player->getPosition().distance(marlon->getPosition());

                if (distance < 50.0f) {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                }
                else {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
            if (maru) {
                const float distance = player->getPosition().distance(maru->getPosition());

                if (distance < 120.0f) {
                    if (player->getCurrentTool() == Player::ToolType::GIFT) {
                        Director::getInstance()->getOpenGLView()->setCursor("cursor_gift.png");
                    }
                    else {
                        Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                    }
                }
                else {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
            if (alex) {
                const float distance = player->getPosition().distance(alex->getPosition());

                if (distance < 50.0f) {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                }
                else {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
        };

    mouseListener->onMouseDown = [=](Event* event)
        {
            const EventMouse* e = (EventMouse*)event;
            const Vec2 clickPos = e->getLocation(); // 获取点击位置

            // 鼠标点击时触发开垦
            if (player && player->getCurrentTool() == Player::ToolType::SHOVEL)
            {
                CropManager::getInstance()->onMouseDown(clickPos, player);
            }
            // 鼠标点击时触发浇水
            if (player && player->getCurrentTool() == Player::ToolType::WATERING)
            {
                CropManager::getInstance()->onMouseDown(clickPos, player);
            }
            // 鼠标点击时触发资源移除
            if (player && player->getCurrentTool() == Player::ToolType::AXE)
            {
                CropManager::getInstance()->onMouseDown(clickPos, player);
            }

            // 检查是否靠近并点击了刘易斯
            if (lewis) {
                const  float distance = player->getPosition().distance(lewis->getPosition());

                if (distance < 50.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作

                    handleQuestDialogue(lewis);

                    // 处理礼物逻辑
                    if (player->getCurrentTool() == Player::ToolType::GIFT) {
                        lewis->stopAllActions();
                        lewis->showThanks();
                        dialogueBox = DialogueBox::create("I love this! Thank you! Mmmmmmm......", "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
                        this->addChild(dialogueBox, 10);

                        Size visibleSize = Director::getInstance()->getVisibleSize();

                        auto popup = cocos2d::Label::createWithSystemFont("Heartpoint with Lewis + 3", "Arial", 24);
                        popup->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 650);
                        popup->setAnchorPoint(Vec2(0.5f, 0.5f)); // 设置锚点为中心
                        popup->setColor(cocos2d::Color3B::BLACK);
                        this->addChild(popup, 100);

                        // 弹窗消失
                        auto fadeOutPopup = FadeOut::create(5.0f);
                        auto removePopup = RemoveSelf::create();
                        popup->runAction(Sequence::create(fadeOutPopup, removePopup, nullptr));

                        if (lewis->getHeartPoints() == 10) {
                            dialogueBox = DialogueBox::create("We are best friends!", "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
                            this->addChild(dialogueBox, 10);
                        }
                    }
                    else {
                        lewis->staticAnimation(); // 静止状态
                    }
                }
                else {
                    player->setCanPerformAction(true);  // 允许玩家动作
                }
            }
            // 检查是否靠近并点击了马龙
            if (marlon) {
                const float distance = player->getPosition().distance(marlon->getPosition());

                if (distance < 50.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    dialogueBox = DialogueBox::create(marlon->getRandomDialogue(), "Portraits/Marlon.png", "Marlon", marlon->getHeartPoints());
                    this->addChild(dialogueBox, 10);
                }
                else {
                    player->setCanPerformAction(true);  // 允许玩家动作
                }
            }
            // 检查是否靠近并点击了玛鲁
            if (maru) {
                const float distance = player->getPosition().distance(maru->getPosition());

                if (distance < 150.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));
                    dialogueBox = DialogueBox::create(maru->getRandomDialogue(), "Portraits/Maru_Hospital.png", "Maru", maru->getHeartPoints());
                    this->addChild(dialogueBox, 10);

                    // 处理礼物逻辑
                    if (player->getCurrentTool() == Player::ToolType::GIFT) {

                        // 好感度增加3
                        maru->heartPoint += 3;
                        dialogueBox = DialogueBox::create("Wow Yummy!", "Portraits/Maru.png", "Maru", maru->getHeartPoints());
                        this->addChild(dialogueBox, 10);
                        Size visibleSize = Director::getInstance()->getVisibleSize();

                        auto popup = cocos2d::Label::createWithSystemFont("Heartpoint with Maru + 3", "Arial", 24);
                        popup->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 650);
                        popup->setAnchorPoint(Vec2(0.5f, 0.5f)); // 设置锚点为中心
                        popup->setColor(cocos2d::Color3B::BLACK);
                        this->addChild(popup, 100);

                        // 弹窗消失
                        auto fadeOutPopup = FadeOut::create(5.0f);
                        auto removePopup = RemoveSelf::create();
                        popup->runAction(Sequence::create(fadeOutPopup, removePopup, nullptr));

                        if (maru->getHeartPoints() == 10) {
                            dialogueBox = DialogueBox::create("I love you forever!!!", "Portraits/Maru.png", "Maru", maru->getHeartPoints());
                            this->addChild(dialogueBox, 10);
                        }
                    }
                    else {
                        maru->staticAnimation();
                    }
                }
                else {
                   player->setCanPerformAction(true);  // 允许玩家动作
                }
            }
            // 检查是否靠近并点击了艾利克斯
            if (alex) {
                float distance = player->getPosition().distance(alex->getPosition());

                if (distance < 50.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    dialogueBox = DialogueBox::create(alex->getRandomDialogue(), "Portraits/Alex.png", "Alex", alex->getHeartPoints());
                    this->addChild(dialogueBox, 10);
                }
                else {
                    player->setCanPerformAction(true);  // 允许玩家动作
                }
            }
            // 检查是否靠近并点击了猪
            if (!pigs.empty()) {  // 确保vector不为空
                for (auto pig : pigs) {  // 遍历所有的pig
                    const float distance = player->getPosition().distance(pig->getPosition());
                    if (distance < 50.0f) {
                        if (player->getCurrentTool() == Player::ToolType::CARROT) {
                            // 如果玩家手持胡萝卜，触发吃饱了动画
                            pig->showFull();
                        }
                        break;
                    }
                }
            }
            // 检查是否靠近并点击了鸡
            if (!chickens.empty()) {  // 确保vector不为空
                for (auto chicken : chickens) {
                    const  float distance = player->getPosition().distance(chicken->getPosition());
                    if (distance < 50.0f) {
                        if (player->getCurrentTool() == Player::ToolType::CARROT) {
                            // 如果玩家手持胡萝卜，触发吃饱了动画
                            chicken->showFull();
                        }
                        break;
                    }
                }
            }
            // 检查是否靠近并点击了羊
            if (!sheeps.empty()) {  // 确保vector不为空
                for (auto sheep : sheeps) {
                    const float distance = player->getPosition().distance(sheep->getPosition());
                    if (distance < 50.0f) {
                        if (player->getCurrentTool() == Player::ToolType::CARROT) {
                            // 如果玩家手持胡萝卜，触发吃饱了动画
                            sheep->showFull();
                        }
                        break;
                    }
                }
            }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void GameScene::initLewis()
{
    // 创建刘易斯
    lewis = Lewis::create();

    if (lewis == nullptr)
    {
        return;
    }

    // 设置刘易斯初始位置
    Vec2 lewis_tilePos = Vec2(15, 16);
    Vec2 lewis_worldPos = _gameMap->convertToWorldCoord(lewis_tilePos);
    lewis->setPosition(lewis_worldPos);

    this->addChild(lewis, 1);

    //设置刘易斯默认移动路径
    Vec2 movePath1_tilePos = Vec2(13, 16);
    Vec2 movePath2_tilePos = Vec2(18, 16);

    Vec2 movePath1_worldPos = _gameMap->convertToWorldCoord(movePath1_tilePos);
    Vec2 movePath2_worldPos = _gameMap->convertToWorldCoord(movePath2_tilePos);
    lewis->path.push_back(movePath1_worldPos);
    lewis->path.push_back(movePath2_worldPos);
    isLewisCreated = true; // 设置标志为已创建

    // 检查当前日期和任务状态
    auto gameTime = GameTime::getInstance();
    const  auto questSystem = QuestSystem::getInstance();
    const int currentDay = gameTime->getDay();

    // 第一天检查木头任务
    if (currentDay == 1 &&
        questSystem->getQuestState(QuestType::COLLECT_WOOD) == QuestState::NOT_STARTED) {
        showQuestMark(lewis);
    }
    // 第二天检查修桥任务
    else if (currentDay == 2 &&
        questSystem->getQuestState(QuestType::REPAIR_BRIDGE) == QuestState::NOT_STARTED) {
        showQuestMark(lewis);
    }
}

void GameScene::initMarlon()
{
    // 创建马龙
    marlon = Marlon::create();

    if (marlon == nullptr)
    {
        return;
    }

    // 设置马龙初始位置
    const  Vec2 marlon_tilePos = Vec2(12, 10);
    const Vec2 marlon_worldPos = _gameMap->convertToWorldCoord(marlon_tilePos);
    marlon->setPosition(marlon_worldPos);

    this->addChild(marlon, 1);
    isMarlonCreated = true; // 设置标志为已创建
}

void GameScene::initMaru()
{
    // 创建玛鲁
    maru = Maru::create();

    if (maru == nullptr)
    {
        return;
    }

    // 设置玛鲁初始位置
    const Vec2 maru_tilePos = Vec2(7, 14);
    const Vec2 maru_worldPos = _gameMap->convertToWorldCoord(maru_tilePos);
    maru->setPosition(maru_worldPos);

    this->addChild(maru, 1);
    isMaruCreated = true; // 设置标志为已创建
}

void GameScene::initAlex()
{
    // 创建艾利克斯
    alex = Alex::create();

    if (alex == nullptr)
    {
        return;
    }

    // 设置艾利克斯初始位置
    const Vec2 alex_tilePos = Vec2(21, 20);
    const Vec2 alex_worldPos = _gameMap->convertToWorldCoord(alex_tilePos);
    alex->setPosition(alex_worldPos);

    this->addChild(alex, 1);
    alex->initializeAnimations();
    isAlexCreated = true; // 设置标志为已创建
}

void GameScene::createPig(const Vec2& initialPosition, const std::vector<Vec2>& path)
{
    Pig* newPig = Pig::create(); // 创建新的猪实例
    if (newPig) {
        newPig->setPosition(initialPosition); // 设置初始位置
        // 设置猪初始位置
        const Vec2 newpig_tilePos = Vec2(initialPosition);
        const Vec2 newpig_worldPos = _gameMap->convertToWorldCoord(newpig_tilePos);
        newPig->setPosition(newpig_worldPos);
        newPig->setPath(path); // 设置路径
        this->addChild(newPig, 1); // 将猪添加到场景中
        pigs.push_back(newPig); // 将新猪添加到向量中
        CCLOG("Pig created successfully at position: (%.1f, %.1f)", newPig->getPosition().x, newPig->getPosition().y);
    }
    else {
        CCLOG("Failed to create Pig instance.");
        return;
    }
}
// 重载 createPig 方法，用于创建静止猪
void GameScene::createPig(const Vec2& initialPosition)
{
    Pig* staticPig = Pig::create(); // 创建新的猪实例
    if (staticPig) {
        staticPig->setPosition(initialPosition); // 设置初始位置
        // 设置猪初始位置
        const Vec2 staticpig_tilePos = Vec2(initialPosition);
        const Vec2 staticpig_worldPos = _gameMap->convertToWorldCoord(staticpig_tilePos);
        staticPig->setPosition(staticpig_worldPos);
        this->addChild(staticPig, 1); // 将静止猪添加到场景中
        pigs.push_back(staticPig); // 将静止猪添加到向量中
        staticPig->staticAnimation(); // 播放静止动画
        CCLOG("Static pig created at position: (%.1f, %.1f)", staticPig->getPosition().x, staticPig->getPosition().y);
    }
    else {
        CCLOG("Failed to create static Pig instance.");
        return;
    }
}

void GameScene::initPig()
{
    isPigCreated = true; // 设置标志为已创建
    // 创建第一只猪
    const Vec2 movePig1Path1_tilePos = Vec2(11, 14);
    const Vec2 movePig1Path2_tilePos = Vec2(11, 18);

    const Vec2 movePig1Path1_worldPos = _gameMap->convertToWorldCoord(movePig1Path1_tilePos);
    const  Vec2 movePig1Path2_worldPos = _gameMap->convertToWorldCoord(movePig1Path2_tilePos);
    std::vector<Vec2> pig1Path = { movePig1Path1_worldPos,movePig1Path2_worldPos };
    createPig(Vec2(11, 16), pig1Path); // 第一只猪的位置和路径

    // 创建第二只猪
    const Vec2 movePig2Path1_tilePos = Vec2(6, 14);
    const Vec2 movePig2Path2_tilePos = Vec2(10, 14);

    const Vec2 movePig2Path1_worldPos = _gameMap->convertToWorldCoord(movePig2Path1_tilePos);
    const Vec2 movePig2Path2_worldPos = _gameMap->convertToWorldCoord(movePig2Path2_tilePos);
    std::vector<Vec2> pig2Path = { movePig2Path1_worldPos,movePig2Path2_worldPos };
    createPig(Vec2(9, 14), pig2Path); // 第二只猪的位置和路径

    // 创建第三只猪
    const Vec2 movePig3Path1_tilePos = Vec2(14, 19);
    const Vec2 movePig3Path2_tilePos = Vec2(14, 17);

    const Vec2 movePig3Path1_worldPos = _gameMap->convertToWorldCoord(movePig3Path1_tilePos);
    const Vec2 movePig3Path2_worldPos = _gameMap->convertToWorldCoord(movePig3Path2_tilePos);
    std::vector<Vec2> pig3Path = { movePig3Path1_worldPos,movePig3Path2_worldPos };
    createPig(Vec2(14, 18), pig3Path); // 第三只猪的位置和路径

    // 创建第四只猪，保持静止
    createPig(Vec2(13, 13)); // 第四只猪的位置
}

void GameScene::createChicken(const Vec2& initialPosition, const std::vector<Vec2>& path)
{
    Chicken* newChicken = Chicken::create(); // 创建新的鸡实例
    if (newChicken) {
        newChicken->setPosition(initialPosition); // 设置初始位置
        // 设置鸡初始位置
        const Vec2 newChicken_tilePos = Vec2(initialPosition);
        const Vec2 newChicken_worldPos = _gameMap->convertToWorldCoord(newChicken_tilePos);
        newChicken->setPosition(newChicken_worldPos);
        newChicken->setPath(path); // 设置路径
        this->addChild(newChicken, 1); // 将鸡添加到场景中
        chickens.push_back(newChicken); // 将新鸡添加到向量中
        CCLOG("Chicken created successfully at position: (%.1f, %.1f)", newChicken->getPosition().x, newChicken->getPosition().y);
    }
    else {
        CCLOG("Failed to create Chicken instance.");
        return;
    }
}
// 重载 createPig 方法，用于创建静止鸡
void GameScene::createChicken(const Vec2& initialPosition)
{
    Chicken* staticChicken = Chicken::create(); // 创建新的鸡实例
    if (staticChicken) {
        staticChicken->setPosition(initialPosition); // 设置初始位置
        // 设置鸡初始位置
        const Vec2 staticChicken_tilePos = Vec2(initialPosition);
        const Vec2 staticChicken_worldPos = _gameMap->convertToWorldCoord(staticChicken_tilePos);
        staticChicken->setPosition(staticChicken_worldPos);
        this->addChild(staticChicken, 1); // 将静止猪\鸡添加到场景中
        chickens.push_back(staticChicken); // 将静止猪添加到向量中
        staticChicken->staticAnimation(); // 播放静止动画
        CCLOG("Static chicken created at position: (%.1f, %.1f)", staticChicken->getPosition().x, staticChicken->getPosition().y);
    }
    else {
        CCLOG("Failed to create static Chicken instance.");
        return;
    }
}

void GameScene::initChicken()
{
    isChickenCreated = true; // 设置标志为已创建
    // 创建第一只鸡
    const Vec2 moveChicken1Path1_tilePos = Vec2(26, 16);
    const Vec2 moveChicken1Path2_tilePos = Vec2(30, 16);

    const Vec2 moveChicken1Path1_worldPos = _gameMap->convertToWorldCoord(moveChicken1Path1_tilePos);
    const Vec2 moveChicken1Path2_worldPos = _gameMap->convertToWorldCoord(moveChicken1Path2_tilePos);
    std::vector<Vec2> Chicken1Path = { moveChicken1Path1_worldPos,moveChicken1Path2_worldPos };
    createChicken(Vec2(28, 16), Chicken1Path); // 第一只鸡的位置和路径

    // 创建第二只鸡，保持静止
    createChicken(Vec2(30, 13)); // 第二只鸡的位置
}

void GameScene::createSheep(const Vec2& initialPosition, const std::vector<Vec2>& path)
{
    Sheep* newSheep = Sheep::create(); // 创建新的羊实例
    if (newSheep) {
        newSheep->setPosition(initialPosition); // 设置初始位置
        // 设置羊初始位置
        const Vec2 newSheep_tilePos = Vec2(initialPosition);
        const Vec2 newSheep_worldPos = _gameMap->convertToWorldCoord(newSheep_tilePos);
        newSheep->setPosition(newSheep_worldPos);
        newSheep->setPath(path); // 设置路径
        this->addChild(newSheep, 1); // 将羊添加到场景中
        sheeps.push_back(newSheep); // 将新羊添加到向量中
        CCLOG("Sheep created successfully at position: (%.1f, %.1f)", newSheep->getPosition().x, newSheep->getPosition().y);
    }
    else {
        CCLOG("Failed to create Sheep instance.");
        return;
    }
}
// 重载 createSheep 方法，用于创建静止羊
void GameScene::createSheep(const Vec2& initialPosition)
{
    Sheep* staticSheep = Sheep::create(); // 创建新的羊实例
    if (staticSheep) {
        staticSheep->setPosition(initialPosition); // 设置初始位置
        // 设置羊初始位置
        const  Vec2 staticSheep_tilePos = Vec2(initialPosition);
        const Vec2 staticSheep_worldPos = _gameMap->convertToWorldCoord(staticSheep_tilePos);
        staticSheep->setPosition(staticSheep_worldPos);
        this->addChild(staticSheep, 1); // 将静止羊添加到场景中
        sheeps.push_back(staticSheep); // 将静止羊添加到向量中
        staticSheep->staticAnimation(); // 播放静止动画
        CCLOG("Static sheep created at position: (%.1f, %.1f)", staticSheep->getPosition().x, staticSheep->getPosition().y);
    }
    else {
        CCLOG("Failed to create static Sheep instance.");
        return;
    }
}

void GameScene::initSheep()
{
    isSheepCreated = true; // 设置标志为已创建
    // 创建第一只羊
    const  Vec2 moveSheep1Path1_tilePos = Vec2(28, 17);
    const  Vec2 moveSheep1Path2_tilePos = Vec2(28, 22);

    const Vec2 moveSheep1Path1_worldPos = _gameMap->convertToWorldCoord(moveSheep1Path1_tilePos);
    const Vec2 moveSheep1Path2_worldPos = _gameMap->convertToWorldCoord(moveSheep1Path2_tilePos);
    std::vector<Vec2> sheep1Path = { moveSheep1Path1_worldPos,moveSheep1Path2_worldPos };
    createSheep(Vec2(28, 20), sheep1Path); // 第一只羊的位置和路径

    // 创建第二只羊
    const  Vec2 moveSheep2Path1_tilePos = Vec2(23, 19);
    const Vec2 moveSheep2Path2_tilePos = Vec2(27, 19);

    const Vec2 moveSheep2Path1_worldPos = _gameMap->convertToWorldCoord(moveSheep2Path1_tilePos);
    const Vec2 moveSheep2Path2_worldPos = _gameMap->convertToWorldCoord(moveSheep2Path2_tilePos);
    std::vector<Vec2> sheep2Path = { moveSheep2Path1_worldPos,moveSheep2Path2_worldPos };
    createSheep(Vec2(26, 19), sheep2Path); // 第二只羊的位置和路径

    // 创建第三只羊，保持静止
    createSheep(Vec2(26, 23)); // 第三只羊的位置
}
// 创建树
void GameScene::createTree(const Vec2& initialPosition)
{
    Tree* tree = Tree::create("TileSheets/fruitTrees.png", 100);
    if (tree) {
        const Vec2 tree_worldPos = _gameMap->convertToWorldCoord(initialPosition);
        tree->setPosition(tree_worldPos);

        // 设置树木被砍倒后的回调
        tree->setOnTreeChoppedCallback([this, tree, tree_worldPos]() {
            // 创建木头掉落物
            auto woodSprite = Sprite::create("TileSheets/fruitTrees.png");
            if (woodSprite) {
                woodSprite->setTextureRect(Rect(384, 700, 32, 32));
                woodSprite->setPosition(tree_worldPos);
                woodSprite->setScale(1.5f);
                this->addChild(woodSprite, 2);

                // 添加点击事件
                auto listener = EventListenerTouchOneByOne::create();
                listener->setSwallowTouches(true);

                listener->onTouchBegan = [this, woodSprite](Touch* touch, Event* event) {
                    Vec2 touchPos = touch->getLocation();
                    touchPos = this->convertToNodeSpace(touchPos);

                    if (woodSprite->getBoundingBox().containsPoint(touchPos)) {
                        // 播放拾取动画
                        auto fadeOut = FadeOut::create(0.3f);
                        auto moveUp = MoveBy::create(0.3f, Vec2(0, 20));
                        auto spawn = Spawn::create(fadeOut, moveUp, nullptr);
                        auto remove = RemoveSelf::create();

                        // 添加木头到物品系统
                        auto addItem = CallFunc::create([]() {
                            ItemSystem::getInstance()->addItem("wood", 10);
                            });

                        woodSprite->runAction(Sequence::create(
                            spawn,
                            addItem,
                            remove,
                            nullptr
                        ));
                        return true;
                    }
                    return false;
                    };

                Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, woodSprite);

                // 添加掉落动画
                const  auto dropDistance = 50.0f;
                const  auto dropDuration = 0.5f;
                woodSprite->setPositionY(woodSprite->getPositionY() + dropDistance);
                auto dropAction = EaseOut::create(
                    MoveBy::create(dropDuration, Vec2(0, -dropDistance)),
                    2.0f
                );
                woodSprite->runAction(dropAction);
            }

            // 移除树木
            tree->removeFromParent();
            auto iter = std::find(trees.begin(), trees.end(), tree);
            if (iter != trees.end()) {
                trees.erase(iter);
            }
            });

        this->addChild(tree, 1);
        trees.push_back(tree);
    }
}

void GameScene::initTree()
{
    if (!isTreeCreated) {
        createTree(Vec2(22, 13));
        createTree(Vec2(36, 15));
        createTree(Vec2(35, 21));
    }
    isTreeCreated = true; // 设置标志为已创建
}
// 创建矿石
void GameScene::createOre(const Vec2& initialPosition)
{
    Ore* ore = Ore::create("TileSheets/Objects_2.png", 10); // 假设矿石的初始生命值为10
    ore->setTextureRect(cocos2d::Rect(65, 112, 16, 16));
    if (ore) {
        Vec2 ore_worldPos = _gameMap->convertToWorldCoord(initialPosition);
        ore->setPosition(ore_worldPos);

        // 设置矿石被挖后的回调
        ore->setOnOreDug([this, ore, ore_worldPos]() {
            // 创建矿石掉落物
            auto stoneSprite = Sprite::create("TileSheets/Objects_2.png");
            if (stoneSprite) {
                stoneSprite->setTextureRect(Rect(65, 112, 16, 16)); // 矿石的纹理
                stoneSprite->setPosition(ore_worldPos);
                stoneSprite->setScale(1.5f);
                this->addChild(stoneSprite, 2);

                // 添加点击事件
                auto listener = EventListenerTouchOneByOne::create();
                listener->setSwallowTouches(true);

                listener->onTouchBegan = [this, stoneSprite](Touch* touch, Event* event) {
                    Vec2 touchPos = touch->getLocation();
                    touchPos = this->convertToNodeSpace(touchPos);

                    if (stoneSprite->getBoundingBox().containsPoint(touchPos)) {
                        // 播放拾取动画
                        auto fadeOut = FadeOut::create(0.3f);
                        auto moveUp = MoveBy::create(0.3f, Vec2(0, 20));
                        auto spawn = Spawn::create(fadeOut, moveUp, nullptr);
                        auto remove = RemoveSelf::create();

                        // 添加矿石到物品系统
                        auto addItem = CallFunc::create([]() {
                            ItemSystem::getInstance()->addItem("stone", 10); // 假设添加10个矿石
                            });

                        stoneSprite->runAction(Sequence::create(
                            spawn,
                            addItem,
                            remove,
                            nullptr
                        ));
                        return true;
                    }
                    return false;
                    };

                Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, stoneSprite);

                // 添加掉落动画
                auto dropDistance = 50.0f;
                auto dropDuration = 0.5f;
                stoneSprite->setPositionY(stoneSprite->getPositionY() + dropDistance);
                auto dropAction = EaseOut::create(
                    MoveBy::create(dropDuration, Vec2(0, -dropDistance)),
                    2.0f
                );
                stoneSprite->runAction(dropAction);
            }

            // 移除矿石
            ore->removeFromParent();
            auto iter = std::find(ores.begin(), ores.end(), ore);
            if (iter != ores.end()) {
                ores.erase(iter);
            }
            });

        this->addChild(ore, 1);
        ores.push_back(ore); 
    }
}

void GameScene::initOre()
{
    if (!isOreCreated) {
        // 矿洞右边
        for (int i = 0; i < 8; i++) {
            createOre(Vec2(17, 10+i));
        }
        // 矿洞左边
        for (int i = 0; i < 9; i++) {
            createOre(Vec2(3, 9+i));
        }
    }
    isOreCreated = true; // 设置标志为已创建
}
/*
 * 初始化宝箱
 * 功能：在地图上创建宝箱
 */
void GameScene::initChests()
{
    if (!_gameMap) {
        CCLOG("GameMap is null");
        return;
    }

    auto objectGroup = _gameMap->getTileMap()->getObjectGroup("Chest");
    if (!objectGroup) {
        CCLOG("Failed to get Chest object group in map: %s", _gameMap->getMapName().c_str());
        return;
    }

    // 获取所有宝箱对象
    auto& objects = objectGroup->getObjects();
    CCLOG("Found %d chest objects in map: %s", (int)objects.size(), _gameMap->getMapName().c_str());

    for (auto& obj : objects)
    {
        auto& dict = obj.asValueMap();
        const float x = dict["x"].asFloat();
        const  float y = dict["y"].asFloat();

        // 创建宝箱
        auto chest = Chest::create();
        if (chest)
        {
            // 获取地图的瓦片大小和缩放比例
            const Size tileSize = _gameMap->getTileMap()->getTileSize();
            const  float scale = _gameMap->getTileMap()->getScale();

            // 将 Tiled 对象坐标转换为瓦片坐标
            const float tileX = x / tileSize.width;
            const float tileY = y / tileSize.height;

            CCLOG("Converting chest position: Tiled(%.1f, %.1f) -> Tile(%.1f, %.1f)",
                x, y, tileX, tileY);

            // 使用 GameMap 的转换方法获取世界坐标
            const Vec2 worldPos = _gameMap->convertToWorldCoord(Vec2(tileX, tileY));

            chest->setPosition(worldPos);
            chest->setScale(2.0f);
            chest->initTouchEvents();

            // 添加到场景
            this->addChild(chest, 1);
            _chests.push_back(chest);


            CCLOG("Placed chest at world position (%.1f, %.1f)", worldPos.x, worldPos.y);
        }
    }
}

void GameScene::clearChests()
{
    for (auto chest : _chests) {
        if (chest) {
            chest->removeFromParent();
        }
    }
    _chests.clear();
    CCLOG("All treasure chests have been cleared");
}

void GameScene::showQuestMark(Node* target) {
    if (!_questMark) {
        // 使用 emojis.png 中的感叹号图标
        _questMark = Sprite::create("LooseSprites/emojis.png");
        if (_questMark) {
            _questMark->setTextureRect(Rect(54, 81, 9, 9));  // 设置感叹号号的纹理区域
            _questMark->setScale(1.5f);
            _questMark->setPosition(Vec2(10, target->getContentSize().height ));
            target->addChild(_questMark);

            // 创建上下浮动的动画
            auto moveUp = MoveBy::create(0.5f, Vec2(0, 10));
            auto moveDown = moveUp->reverse();
            auto sequence = Sequence::create(moveUp, moveDown, nullptr);
            auto repeat = RepeatForever::create(sequence);
            _questMark->runAction(repeat);
        }
    }
    if (_questMark) {
        _questMark->setVisible(true);
    }
}

void GameScene::hideQuestMark(Node* target) {
    if (_questMark) {
        _questMark->setVisible(false);
    }
}

void GameScene::updateQuestUI() {
    if (_questTipLabel) {
        auto questSystem = QuestSystem::getInstance();
        auto gameTime = GameTime::getInstance();
        int currentDay = gameTime->getDay();

        if (currentDay == 1 &&
            questSystem->getQuestState(QuestType::COLLECT_WOOD) == QuestState::IN_PROGRESS) {
            // 木头收集任务UI
            auto& questData = questSystem->getQuestData(QuestType::COLLECT_WOOD);
            int woodCount = ItemSystem::getInstance()->getItemCount("wood");
            _questTipLabel->setString(questData.title + ": " +
                std::to_string(woodCount) + "/" +
                std::to_string(questData.targetAmount));
        }
        else if (currentDay == 2 &&
            questSystem->getQuestState(QuestType::REPAIR_BRIDGE) == QuestState::IN_PROGRESS) {
            // 修桥任务UI
            auto& questData = questSystem->getQuestData(QuestType::REPAIR_BRIDGE);
            _questTipLabel->setString(questData.title);
        }
    }
}

void GameScene::handleQuestDialogue(Lewis* lewis) {
    if (!lewis) return;

    auto questSystem = QuestSystem::getInstance();
    if (!questSystem) return;

    auto gameTime = GameTime::getInstance();
    int currentDay = gameTime->getDay();

    // 第一天的木头收集任务
    if (currentDay == 1) {
        auto woodQuestState = questSystem->getQuestState(QuestType::COLLECT_WOOD);
        if (woodQuestState != QuestState::COMPLETED) {  // 只有未完成时才处理任务
            handleWoodQuest(lewis, woodQuestState);
            return;
        }
    }
    // 第二天的修桥任务
    else if (currentDay == 2) {
        auto bridgeQuestState = questSystem->getQuestState(QuestType::REPAIR_BRIDGE);
        if (bridgeQuestState != QuestState::COMPLETED) {  // 只有未完成时才处理任务
            handleBridgeQuest(lewis, bridgeQuestState);
            return;
        }
    }

    // 如果当天的任务已完成或是其他天，显示随机对话
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    dialogueBox = DialogueBox::create(lewis->getRandomDialogue(), "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
    this->addChild(dialogueBox, 10);
}

void GameScene::handleWoodQuest(Lewis* lewis, QuestState questState) {
    auto questSystem = QuestSystem::getInstance();

    if (questState == QuestState::NOT_STARTED) {
        // 显示任务介绍对话
        auto& questData = questSystem->getQuestData(QuestType::COLLECT_WOOD);
        if (!dialogueBox) {  // 检查是否已存在对话框
            dialogueBox = DialogueBox::create(questData.description, "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
            if (dialogueBox) {  // 检查创建是否成功
                this->addChild(dialogueBox, 10);
            }
        }

        // 创建任务提示UI，使用固定位置
        if (!_questTipLabel) {
            _questTipLabel = Label::createWithTTF("", "fonts/arial.ttf", 24);
            if (_questTipLabel) {
                const Size visibleSize = Director::getInstance()->getVisibleSize();
                // 将Label添加为HUD层的子节点
                this->addChild(_questTipLabel, 10);
                _questTipLabel->setAnchorPoint(Vec2(1, 1)); // 设置右上角为锚点
                updateQuestUIPosition();
            }
        }

        // 开始任务
        questSystem->startQuest(QuestType::COLLECT_WOOD);
        hideQuestMark(lewis);
        updateQuestUI();
    }
    else if (questState == QuestState::IN_PROGRESS) {
        const int woodCount = ItemSystem::getInstance()->getItemCount("wood");
        auto& questData = questSystem->getQuestData(QuestType::COLLECT_WOOD);

        if (woodCount >= questData.targetAmount) {
            dialogueBox = DialogueBox::create(
                "Great! Thank you!",
                "Portraits/Lewis.png",
                "Lewis",
                lewis->getHeartPoints()
            );
            this->addChild(dialogueBox, 10);

            // 完成任务
            ItemSystem::getInstance()->removeItem("wood", questData.targetAmount);
            questSystem->completeQuest(QuestType::COLLECT_WOOD);

            // 移除任务提示UI
            if (_questTipLabel) {
                _questTipLabel->removeFromParent();
                _questTipLabel = nullptr;
            }
        }
        else {
            dialogueBox = DialogueBox::create(
                "Go on, I need " + std::to_string(questData.targetAmount - woodCount) + " more.",
                "Portraits/Lewis.png",
                "Lewis",
                lewis->getHeartPoints()
            );
            this->addChild(dialogueBox, 10);
        }
    }
    else if (questState == QuestState::COMPLETED) {
        // 任务完成后显示随机对话
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        dialogueBox = DialogueBox::create(lewis->getRandomDialogue(), "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
        this->addChild(dialogueBox, 10);
    }
}

void GameScene::handleBridgeQuest(Lewis* lewis, QuestState questState) {
    auto questSystem = QuestSystem::getInstance();

    if (questState == QuestState::NOT_STARTED) {
        // 显示任务介绍对话
        auto& questData = questSystem->getQuestData(QuestType::REPAIR_BRIDGE);
        dialogueBox = DialogueBox::create(questData.description, "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
        this->addChild(dialogueBox, 10);

        // 创建任务提示UI
        if (!_questTipLabel) {
            _questTipLabel = Label::createWithTTF("", "fonts/arial.ttf", 24);
            if (_questTipLabel) {
                this->addChild(_questTipLabel, 10);
                _questTipLabel->setAnchorPoint(Vec2(1, 1));
                updateQuestUIPosition();
            }
        }

        // 开始任务
        questSystem->startQuest(QuestType::REPAIR_BRIDGE);
        hideQuestMark(lewis);  // 接受任务后隐藏感叹号
        updateQuestUI();
    }
    else if (questState == QuestState::IN_PROGRESS) {
        dialogueBox = DialogueBox::create(
            "Come on! We will reach the other side of river.",
            "Portraits/Lewis.png",
            "Lewis",
            lewis->getHeartPoints()
        );
        this->addChild(dialogueBox, 10);

    }
    else if (questState == QuestState::COMPLETED) {
        static bool isFirstCompletion = true;  // 静态变量记录是否是首次完成

        if (isFirstCompletion) {
            // 首次完成显示特殊对话
            dialogueBox = DialogueBox::create(
                "Great job! Now we can get there!",
                "Portraits/Lewis.png",
                "Lewis",
                lewis->getHeartPoints()
            );
            isFirstCompletion = false;  // 设置为非首次
            this->addChild(dialogueBox, 10);
        }
        else {
            // 后续显示随机对话
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            dialogueBox = DialogueBox::create(lewis->getRandomDialogue(), "Portraits/Lewis.png", "Lewis", lewis->getHeartPoints());
            this->addChild(dialogueBox, 10);
        }

        // 移除任务提示UI
        if (_questTipLabel) {
            _questTipLabel->removeFromParent();
            _questTipLabel = nullptr;
        }
    }
}

// 更新任务UI位置
void GameScene::updateQuestUIPosition() {
    if (_questTipLabel) {
        const Size visibleSize = Director::getInstance()->getVisibleSize();
        const Vec2 origin = Director::getInstance()->getVisibleOrigin();

        // 计算屏幕右上角的位置
        Vec2 position = Vec2(origin.x + visibleSize.width - 20,  // 右边缘留20像素边距
            origin.y + visibleSize.height - 160);   // 上边缘留160像素边距

        // 如果场景有偏移，需要加上偏移量
        position = position - this->getPosition();

        _questTipLabel->setPosition(position);
    }
}

void GameScene::checkQuestProgress() {
    auto questSystem = QuestSystem::getInstance();
    if (questSystem->getQuestState(QuestType::COLLECT_WOOD) == QuestState::IN_PROGRESS) {
        const int woodCount = ItemSystem::getInstance()->getItemCount("wood");
        questSystem->updateQuestProgress(QuestType::COLLECT_WOOD, woodCount);
        updateQuestUI();
    }
}
