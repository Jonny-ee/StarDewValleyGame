#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
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
        CCLOG("初始化失败：无法加载地图");
        return false;
    }
    this->addChild(_gameMap);

    // 创建玩家
    player = Player::create();
    if (player == nullptr)
    {
        return false;
    }
 // 设置关联
    player->setGameMap(_gameMap);

    // 设置玩家初始位置
    Vec2 tilePos = Vec2(23, 6);
    Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
    player->setPosition(worldPos);
    player->setScale(3.0f);

    // 初始化玩家的事件监听
    player->initKeyboardListener();
    player->initMouseListener();

    this->addChild(player, 1);

    // 设置键盘事件监听
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    // 启动更新
    this->scheduleUpdate();

    // 更新相机位置
    updateCamera();

    return true;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 只保留特殊按键的处理（如 B 键传送），其他移动相关的按键交给 Player 处理
    if (keyCode == EventKeyboard::KeyCode::KEY_B) {
        CCLOG("按键事件：按下B键 - 触发传送");
        switchToFarmMap();
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 同上，只保留特殊按键的处理
    if (keyCode == EventKeyboard::KeyCode::KEY_B) {
        // 如果需要在释放时处理
    }
}

void GameScene::update(float dt)
{
    if (!player || !_gameMap) {
        return;
    }

   // 直接调用 player 的 update
    player->update(dt);

    // 检查传送点
    Vec2 playerTilePos = _gameMap->convertToTileCoord(player->getPosition());
    TransitionInfo transition;
    if (_gameMap->checkForTransition(playerTilePos, transition)) {
        switchToMap(transition.targetMap, transition.targetTilePos);
    }
    // 更新相机位置
    updateCamera();
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
    x = std::min(x, (mapSize.width * tileSize.width * scale - visibleSize.width/2));
    y = std::min(y, (mapSize.height * tileSize.height * scale - visibleSize.height/2));

    Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 pointB = Vec2(x, y);
    Vec2 offset = pointA - pointB;
    this->setPosition(offset);
}

void GameScene::switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos)
{
    try {
        // 创建新场景
        auto newScene = GameScene::create();
        if (!newScene) {
            throw std::runtime_error("无法创建新场景");
        }

        // 创建新地图
        newScene->_gameMap = GameMap::create(mapName);
        if (!newScene->_gameMap) {
            throw std::runtime_error("无法创建新地图");
        }
        newScene->addChild(newScene->_gameMap);

        // 创建新玩家
        newScene->player = Player::create();
        if (!newScene->player) {
            throw std::runtime_error("无法创建新玩家");
        }

        // 设置玩家位置
        Vec2 worldPos = newScene->_gameMap->convertToWorldCoord(targetTilePos);
        newScene->player->setPosition(worldPos);
        newScene->player->setScale(3.0f);
        newScene->addChild(newScene->player, 1);

        // 创建淡出效果
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto blackLayer = LayerColor::create(Color4B::BLACK);
        blackLayer->setContentSize(visibleSize);
        newScene->addChild(blackLayer, 999);

        // 切换场景
        Director::getInstance()->replaceScene(newScene);

        auto fadeOut = FadeOut::create(0.5f);
        auto removeSelf = RemoveSelf::create();
        blackLayer->runAction(Sequence::create(fadeOut, removeSelf, nullptr));

        // 更新相机位置
        newScene->updateCamera();
    }
    catch (const std::exception& e) {
        CCLOG("切换地图时发生错误: %s", e.what());
    }
}

void GameScene::switchToFarmMap()
{
    CCLOG("传送：开始切换到Farm地图");
    switchToMap("Farm1", Vec2(29, 11));
}