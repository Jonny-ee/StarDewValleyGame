#include "GameScene.h"
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
            toolIcon->setVisible(false);  // 无工具时隐藏图标
            break;
        case 1:  // SHOVEL
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(32, 0, 16, 16));  // 根据实际图片位置调整
            break;
        case 2:  // AXE
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(16, 0, 16, 16));  // 根据实际图片位置调整
            break;
        case 3:  // WATERING
            toolIcon->setVisible(true);
            toolIcon->setTextureRect(cocos2d::Rect(0, 0, 16, 16));   // 根据实际图片位置调整
            break;
        case 4:  // ROD
            toolIcon->setVisible(false);  // 暂时没有鱼竿素材，隐藏图标
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
    return true;
}

void GameScene::update(float dt)
{
    if (!player || !_gameMap) {
        return;
    }

    // 只保留一次update调用
    player->update(dt);

    // 检查玩家与Lewis的距离
    if (lewis) {
        float distance = player->getPosition().distance(lewis->getPosition());
        if (distance < 50.0f) { // 设定一个阈值
            dialogueBox = DialogueBox::create("Hello,little cat!", "textBox.png");
            this->addChild(dialogueBox, 10);
            //lewis->startConversation(); // 弹出对话框
        }
    }
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

    // 获取玩家位置
    Vec2 playerPos = player->getPosition();

    // 计算目标视角位置（使玩家保持在屏幕中心）
    float x = std::max(playerPos.x, visibleSize.width / 2);
    float y = std::max(playerPos.y, visibleSize.height / 2);
    x = std::min(x, (mapSize.width * tileSize.width * scale - visibleSize.width / 2));
    y = std::min(y, (mapSize.height * tileSize.height * scale - visibleSize.height / 2));

    Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 pointB = Vec2(x, y);
    Vec2 offset = pointA - pointB;
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

    // 移除旧地图
    if (_gameMap) {
        _gameMap->removeFromParent();
        _gameMap = nullptr;
    }

    // 创建新地图
    _gameMap = GameMap::create(mapName);
    if (_gameMap) {
        this->addChild(_gameMap);
    }

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

    // 更新相机位置
    this->updateCamera();
}

void GameScene::initMouseListener()
{
    auto mouseListener = EventListenerMouse::create();

    // 鼠标点击事件
    mouseListener->onMouseDown = [=](Event* event)
        {
            EventMouse* e = (EventMouse*)event;
            Vec2 clickPos = e->getLocation(); // 获取点击位置

            // 检查是否点击了Lewis，点击暂时未实现
            if (lewis && lewis->getBoundingBox().containsPoint(clickPos)) {
                player->setCanPerformAction(false); // 禁止玩家动作
                lewis->startConversation();         // 触发对话
            }
            else {
                player->setCanPerformAction(true);  // 允许玩家动作
            }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}
