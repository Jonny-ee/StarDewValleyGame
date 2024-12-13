#include "GameScene.h"
#include "GameTime.h"
#include"LightManager.h"
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
    // 创建刘易斯
    lewis = Lewis::create();

    if (lewis == nullptr)
    {
        return false;
    }

    // 设置刘易斯初始位置
    Vec2 lewis_tilePos = Vec2(15, 6);
    Vec2 lewis_worldPos = _gameMap->convertToWorldCoord(lewis_tilePos);
    lewis->setPosition(lewis_worldPos);

    this->addChild(lewis, 1);

    //设置刘易斯默认移动路径
    Vec2 movePath1_tilePos = Vec2(13, 6);
    Vec2 movePath2_tilePos = Vec2(18, 6);

    Vec2 movePath1_worldPos = _gameMap->convertToWorldCoord(movePath1_tilePos);
    Vec2 movePath2_worldPos = _gameMap->convertToWorldCoord(movePath2_tilePos);
    lewis->path.push_back(movePath1_worldPos);
    lewis->path.push_back(movePath2_worldPos);

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

        lewis->updateSchedule(dt);
        lewis->moveAlongPath(dt); // 移动沿路径
    }
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
}

void GameScene::switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos)
{
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

    // 如果是矿洞地图，初始化宝箱
    if (mapName == "Mine") {
        CCLOG("Switching to Mine map, initializing chests...");
        initChests();
    }

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
                    // 鼠标靠近Lewis，变成交互光标
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_dialogue.png");
                }
                else {
                    // 鼠标远离Lewis，恢复默认光标
                    Director::getInstance()->getOpenGLView()->setCursor("cursor_default.png");
                }
            }
        };

    mouseListener->onMouseDown = [=](Event* event)
        {
            EventMouse* e = (EventMouse*)event;
            Vec2 clickPos = e->getLocation(); // 获取点击位置

            // 检查是否靠近并点击了Lewis
            if (lewis) {
                float distance = player->getPosition().distance(lewis->getPosition());

                if (distance < 50.0f) {
                    player->setCanPerformAction(false); // 禁止玩家动作
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));

                    dialogueBox = DialogueBox::create(lewis->getRandomDialogue(), "textBox.png");
                    this->addChild(dialogueBox, 10);
                }
                else {
                    player->setCanPerformAction(true);  // 允许玩家动作
                }
            }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
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