#include "GameScene.h"
#include "GameTime.h"
#include"LightManager.h"
#include "Chest.h" 

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
        updateToolIcon();  // 设置初始工具图标
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
            toolIcon->setTexture("tools.png");  // 使用原来的工具贴图
            toolIcon->setVisible(false);        // 没有工具，隐藏贴图
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
        case 3:  // WATERING
            toolIcon->setTexture("tools.png");  // 使用原来的工具贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(0, 0, 16, 16));
            break;
        case 4:  // ROD
            toolIcon->setTexture("TileSheets/Tools.png");  // 使用新的贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(128, 0, 16, 16));
            break;
        case 5:  // GIFT
            toolIcon->setTexture("TileSheets/Objects_2.png");  // 使用新的贴图
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(96, 32, 16, 16));
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
    isLewisCreated = false; // 初始化标志
    isMarlonCreated = false; // 初始化标志
    isMaruCreated = false; // 初始化标志
    isAlexCreated = false; // 初始化标志

    // 初始化矿洞进入时间记录
    lastMineEnterDay = 0;
    lastMineEnterMonth = 0;
    lastMineEnterYear = 0;

    // 创建并加载地图
    _gameMap = GameMap::create("House");
    if (_gameMap == nullptr)
    {
        //CCLOG("初始化失败：无法加载地图");
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
    Vec2 tilePos = Vec2(23, 6);
    Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
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

    // 更新相机位置
    updateCamera();

    // 初始化工具图标
    initToolIcon();

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
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        _statusUI->setPosition(origin.x + visibleSize.width,
            origin.y + visibleSize.height);

        // 添加到最上层，确保不会被其他内容遮挡
        this->addChild(_statusUI, 10);
    }
    return true;
}

void GameScene::update(float dt)
{
    if (!player || !_gameMap) {
        return;
    }
    // 获取GameTime单例实例
    GameTime* gameTime = GameTime::getInstance();
    // 更新游戏时间
    gameTime->update();
    // 更新光照效果
    LightManager::getInstance()->update();
    // 只保留一次update调用
    player->update(dt);

    // 检查传送点
    Vec2 playerTilePos = _gameMap->convertToTileCoord(player->getPosition());
    TransitionInfo transition;
    if (_gameMap->checkForTransition(playerTilePos, transition)) {
        switchToMap(transition.targetMap, transition.targetTilePos);
    }

    updateCamera();
    updateToolIcon();  // 每帧更新工具图标

    // 更新Lewis的状态
    if (lewis) {

        //lewis->updateSchedule(dt);
        lewis->moveAlongPath(dt); // 移动沿路径
    }

    // 持续检查钓鱼条件
    auto fishingSystem = FishingSystem::getInstance();
    fishingSystem->canFish(player->getPosition(), player);

    // 更新提示标签位置
    if (auto tipLabel = fishingSystem->getTipLabel()) {
        if (tipLabel->isVisible()) {
            Vec2 playerPos = player->getPosition();
            // 设置在玩家头顶上方50像素
            tipLabel->setPosition(playerPos + Vec2(0, 50));
        }
    }

    // 检查并执行睡觉事件
    checkAndExecuteSleepEvent();

}

void GameScene::updateCamera()
{
    if (!player || !_gameMap) return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Size mapSize = _gameMap->getTileMap()->getMapSize();
    Size tileSize = _gameMap->getTileMap()->getTileSize();
    float scale = _gameMap->getTileMap()->getScale();

    // 计算地图的实际像素大小
    float mapWidth = mapSize.width * tileSize.width * scale;
    float mapHeight = mapSize.height * tileSize.height * scale;

    // 获取玩家位置
    Vec2 playerPos = player->getPosition();

    float x, y;

    // 如果地图小于屏幕，则居中显示
    if (mapWidth < visibleSize.width) {
        x = visibleSize.width / 2;
    }
    else {
        x = std::max(playerPos.x, visibleSize.width / 2);
        x = std::min(x, mapWidth - visibleSize.width / 2);
    }

    if (mapHeight < visibleSize.height) {
        y = visibleSize.height / 2;
    }
    else {
        y = std::max(playerPos.y, visibleSize.height / 2);
        y = std::min(y, mapHeight - visibleSize.height / 2);
    }

    Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 pointB = Vec2(x, y);
    Vec2 offset = pointA - pointB;


    // 如果地图小于屏幕，调整偏移量使地图居中
    if (mapWidth < visibleSize.width) {
        offset.x = (visibleSize.width - mapWidth) / 2;
    }
    if (mapHeight < visibleSize.height) {
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
    // 更新对话框位置
    if (dialogueBox)
    {
        dialogueBox->setPosition(-offset + Vec2(visibleSize.width / 2, 90));
    }

    // 更新UI位置，确保状态UI始终固定在右上角
    if (_statusUI) {
        _statusUI->setPosition(-offset + Vec2(visibleSize.width, visibleSize.height));
    }
}

void GameScene::switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos)
{
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

    // 加载新地图
    _gameMap->loadMap(mapName);

    // 重用现有玩家，而不是创建新的（修复原来多重玩家的bug）
    if (currentPlayer) {
        Vec2 worldPos = _gameMap->convertToWorldCoord(targetTilePos);
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

    // 如果是农场地图，初始化刘易斯
    if (mapName == "Farm") {
        CCLOG("Switching to Farm map, initializing lewis...");
        initLewis();
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


    // 如果是矿洞地图，检查是否需要刷新宝箱
    if (mapName == "Mine") {
        CCLOG("切换到矿洞地图...");

        auto gameTime = GameTime::getInstance();
        int currentDay = gameTime->getDay();
        int currentMonth = gameTime->getMonth();
        int currentYear = gameTime->getYear();

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
            CCLOG("刷新矿洞宝箱...");
            initChests();
            // 更新进入时间
            lastMineEnterDay = currentDay;
            lastMineEnterMonth = currentMonth;
            lastMineEnterYear = currentYear;
        }
        else {
            CCLOG("今天已经刷新过宝箱，不再刷新");
        }

        // 初始化马龙NPC
        initMarlon();
    }

    // 重新初始化钓鱼系统
    FishingSystem::getInstance()->initFishingAreas(_gameMap);

    // 更新CropManager的地图引用
    CropManager::getInstance()->setGameMap(_gameMap);

    // 更新相机位置
    this->updateCamera();
}

void GameScene::initMouseListener()
{
    auto mouseListener = EventListenerMouse::create();

    // 设置默认光标
    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");

    mouseListener->onMouseMove = [=](Event* event)
        {
            EventMouse* e = (EventMouse*)event;
            Vec2 mousePos = e->getLocation();

            if (lewis) {
                float distance = player->getPosition().distance(lewis->getPosition());

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
                float distance = player->getPosition().distance(marlon->getPosition());

                if (distance < 50.0f) {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                }
                else {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
            if (maru) {
                float distance = player->getPosition().distance(maru->getPosition());

                if (distance < 120.0f) {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                }
                else {
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
            if (alex) {
                float distance = player->getPosition().distance(alex->getPosition());

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
            EventMouse* e = (EventMouse*)event;
            Vec2 clickPos = e->getLocation(); // 获取点击位置

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

            // 检查是否靠近并点击了刘易斯
            if (lewis) {
                float distance = player->getPosition().distance(lewis->getPosition());

                if (distance < 50.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    dialogueBox = DialogueBox::create(lewis->getRandomDialogue(), "Portraits/Lewis.png", "Lewis");
                    this->addChild(dialogueBox, 10);

                    if (player->getCurrentTool() == Player::ToolType::GIFT) {
                        // 如果玩家手持礼物，触发感谢动画
                        lewis->stopAllActions();
                        lewis->showThanks();
                        dialogueBox = DialogueBox::create("I love this! Thank you! Mmmmmmm......", "Portraits/Lewis.png", "Lewis");
                        this->addChild(dialogueBox, 10);

                        //lewis->receiveGift("gift_item"); // 传递礼物项
                       //player->setCurrentTool(Player::ToolType::SHOVEL); // 切换工具
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
                float distance = player->getPosition().distance(marlon->getPosition());

                if (distance < 50.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    dialogueBox = DialogueBox::create(marlon->getRandomDialogue(), "Portraits/Marlon.png", "Marlon");
                    this->addChild(dialogueBox, 10);
                }
                else {
                    player->setCanPerformAction(true);  // 允许玩家动作
                }
            }
            // 检查是否靠近并点击了玛鲁
            if (maru) {
                float distance = player->getPosition().distance(maru->getPosition());

                if (distance < 150.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    dialogueBox = DialogueBox::create(maru->getRandomDialogue(), "Portraits/Maru_Hospital.png", "Maru");
                    this->addChild(dialogueBox, 10);
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

                    dialogueBox = DialogueBox::create(alex->getRandomDialogue(), "Portraits/Alex.png", "Alex");
                    this->addChild(dialogueBox, 10);
                }
                else {
                    player->setCanPerformAction(true);  // 允许玩家动作
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
    Vec2 marlon_tilePos = Vec2(12, 10);
    Vec2 marlon_worldPos = _gameMap->convertToWorldCoord(marlon_tilePos);
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
    Vec2 maru_tilePos = Vec2(7, 14);
    Vec2 maru_worldPos = _gameMap->convertToWorldCoord(maru_tilePos);
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
    Vec2 alex_tilePos = Vec2(21, 20);
    Vec2 alex_worldPos = _gameMap->convertToWorldCoord(alex_tilePos);
    alex->setPosition(alex_worldPos);

    this->addChild(alex, 1);
    alex->initializeAnimations();
    isAlexCreated = true; // 设置标志为已创建
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
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();

        // 创建宝箱
        auto chest = Chest::create();
        if (chest)
        {
            // 获取地图的瓦片大小和缩放比例
            Size tileSize = _gameMap->getTileMap()->getTileSize();
            float scale = _gameMap->getTileMap()->getScale();

            // 将 Tiled 对象坐标转换为瓦片坐标
            float tileX = x / tileSize.width;
            float tileY = y / tileSize.height;

            CCLOG("Converting chest position: Tiled(%.1f, %.1f) -> Tile(%.1f, %.1f)",
                x, y, tileX, tileY);

            // 使用 GameMap 的转换方法获取世界坐标
            Vec2 worldPos = _gameMap->convertToWorldCoord(Vec2(tileX, tileY));

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
    CCLOG("所有宝箱已清理");
}
void GameScene::checkAndExecuteSleepEvent() {
    // 如果已经在执行睡觉事件，则返回
    if ( !player || !_gameMap) return;

    // 获取玩家当前瓦片位置
    Vec2 playerTilePos = _gameMap->convertToTileCoord(player->getPosition());

    // 检查是否在触发位置 (26,6)
    if (_gameMap->getMapName() == "House" &&
        std::abs(playerTilePos.x - 26) < 0.5f &&
        std::abs(playerTilePos.y - 6) < 0.5f) {

        // 禁用玩家输入
        player->setCanPerformAction(false);

        // 计算地图实际大小
        Size mapSize = _gameMap->getTileMap()->getMapSize();
        Size tileSize = _gameMap->getTileMap()->getTileSize();
        float scale = _gameMap->getTileMap()->getScale();
        Size actualMapSize = Size(mapSize.width * tileSize.width * scale,
            mapSize.height * tileSize.height * scale);

        // 创建覆盖整个地图的黑色滤镜
        auto blackFilter = LayerColor::create(Color4B(0, 0, 0, 0));
        blackFilter->setContentSize(actualMapSize);
        // 设置位置与地图对齐
        blackFilter->setPosition(_gameMap->getTileMap()->getPosition());
        _gameMap->addChild(blackFilter, 9999);  // 添加到地图上，确保在最上层

        // 创建睡觉序列
        auto sequence = Sequence::create(
            // 1. 移动到睡觉位置 (25,6)
            CallFunc::create([this]() {
                Vec2 sleepPos = _gameMap->convertToWorldCoord(Vec2(25, 6));
                player->setPosition(sleepPos);
                }),

            // 2. 淡入黑色滤镜
            CallFunc::create([blackFilter]() {
                blackFilter->runAction(FadeTo::create(1.0f, 255));
                }),

            // 3. 等待1秒
            DelayTime::create(1.0f),

            // 4. 修改游戏时间并移动到醒来位置
            CallFunc::create([this]() {
                // 修改游戏时间到第二天早上6点
                GameTime::getInstance()->modifyGameTime(6);

                // 移动到醒来位置 (23,6)
                Vec2 wakeupPos = _gameMap->convertToWorldCoord(Vec2(23, 6));
                player->setPosition(wakeupPos);
                }),
                // 5. 淡出黑色滤镜
                CallFunc::create([blackFilter]() {
                    blackFilter->runAction(Sequence::create(
                        FadeTo::create(1.0f, 0),
                        CallFunc::create([blackFilter]() {
                            blackFilter->removeFromParent();
                            }),
                        nullptr
                    ));
                    }),

                    // 6. 重新启用玩家输入
                    CallFunc::create([this]() {
                    player->setCanPerformAction(true);
                        }),

                    nullptr
                    );

        // 执行动作序列
        player->runAction(sequence);
    }
}
