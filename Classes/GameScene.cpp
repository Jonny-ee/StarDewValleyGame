#include "GameScene.h"
#include "GameMap.h"

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
  
    // 如果_gameMap还没创建（说明是首次进入House地图），才创建House地图
    if (!_gameMap)
    {
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

        // 设置House地图的初始位置
        Vec2 tilePos = Vec2(23, 6);
        Vec2 worldPos = _gameMap->convertToWorldCoord(tilePos);
        player->setPosition(worldPos);
        player->setScale(3.0f);
        this->addChild(player, 1);
    }

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
    CCLOG("按键按下: %d", static_cast<int>(keyCode));
    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_W:
            _pressedKeys.insert('W');
            break;
        case EventKeyboard::KeyCode::KEY_S:
            _pressedKeys.insert('S');
            break;
        case EventKeyboard::KeyCode::KEY_A:
            _pressedKeys.insert('A');
            break;
        case EventKeyboard::KeyCode::KEY_D:
            _pressedKeys.insert('D');
            break;
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    CCLOG("按键释放: %d", static_cast<int>(keyCode));
    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_W:
            _pressedKeys.erase('W');
            break;
        case EventKeyboard::KeyCode::KEY_S:
            _pressedKeys.erase('S');
            break;
        case EventKeyboard::KeyCode::KEY_A:
            _pressedKeys.erase('A');
            break;
        case EventKeyboard::KeyCode::KEY_D:
            _pressedKeys.erase('D');
            break;
        case EventKeyboard::KeyCode::KEY_B:
            CCLOG("按键事件：按下B键 - 触发传送");
            switchToFarmMap();
            break;
    }
}

void GameScene::update(float dt)
{
    if (!this->player || !this->_gameMap) {
        return;
    }

    // 处理移动
    Vec2 moveDirection;
    if (_pressedKeys.find('W') != _pressedKeys.end()) {
        moveDirection.y += 1;
        CCLOG("W键被按下");
    }
    if (_pressedKeys.find('S') != _pressedKeys.end()) {
        moveDirection.y -= 1;
        CCLOG("S键被按下");
    }
    if (_pressedKeys.find('A') != _pressedKeys.end()) {
        moveDirection.x -= 1;
        CCLOG("A键被按下");
    }
    if (_pressedKeys.find('D') != _pressedKeys.end()) {
        moveDirection.x += 1;
        CCLOG("D键被按下");
    }

    // 如果有移动输入
    if (moveDirection != Vec2::ZERO) {
        CCLOG("检测到移动方向: (%.1f, %.1f)", moveDirection.x, moveDirection.y);
        Vec2 currentWorldPos = player->getPosition();
        Vec2 nextPos = currentWorldPos + moveDirection.getNormalized() *
            player->getMoveSpeed() * Director::getInstance()->getDeltaTime();

        if (_gameMap->isWalkable(nextPos)) {
            player->moveInDirection(moveDirection);
            updateCamera();
            CCLOG("玩家移动到位置: (%.1f, %.1f)", player->getPosition().x, player->getPosition().y);
        }
    }

    // 检查传送点
    try {
        Vec2 playerTilePos = this->_gameMap->convertToTileCoord(player->getPosition());
        TransitionInfo transition;

        if (_gameMap->checkForTransition(playerTilePos, transition)) {
            // 清空按键状态
            _pressedKeys.clear();

            // 固定玩家位置
            Vec2 warpPos = _gameMap->convertToWorldCoord(Vec2(transition.sourceX, transition.sourceY));
            player->setPosition(warpPos);

            // 延迟执行场景切换
            auto delay = DelayTime::create(0.5f);
            auto switchFunc = CallFunc::create([this, transition]() {
                this->switchToMap(transition.targetMap, transition.targetTilePos);
                });
            this->runAction(Sequence::create(delay, switchFunc, nullptr));
            return;
        }
    }
    catch (const std::exception& e) {
        CCLOG("传送检测错误: %s", e.what());
    }
}

void GameScene::switchToMap(const std::string& mapName, const cocos2d::Vec2& targetTilePos)
{
    // 创建新场景
    auto newScene = GameScene::create();
    if (!newScene) {
        CCLOG("切换地图失败：无法创建新场景");
        return;
    }

    try {
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



        // 创建一个黑色层用于淡出效果
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto blackLayer = LayerColor::create(Color4B::BLACK);
        blackLayer->setContentSize(visibleSize);
        newScene->addChild(blackLayer, 999);

        // 切换到新场景
        Director::getInstance()->replaceScene(newScene);

        auto fadeOut = FadeOut::create(0.5f);
        auto removeSelf = RemoveSelf::create();
        blackLayer->runAction(Sequence::create(fadeOut, removeSelf, nullptr));

        // 更新相机位置
        newScene->updateCamera();

    }
    catch (const std::exception& e) {
        CCLOG("切换地图时发生错误: %s", e.what());
        return;
    }
}


//此函数为测试函数，切换至农场界面，并且重置玩家位置为地图坐标TildPos（29，11）
void GameScene::switchToFarmMap()
{
    CCLOG("传送：开始切换到Farm地图");

    auto newScene = GameScene::create();
    if (newScene )
    {
        CCLOG("传送：创建新场景成功");
        
        // 创建新地图
        newScene->_gameMap = GameMap::create("Farm1");
        if (newScene->_gameMap)
        {
            CCLOG("传送：新地图创建成功");
            newScene->addChild(newScene->_gameMap);
            CCLOG("传送：地图已添加到场景");

            // 创建新的玩家
            newScene->player = Player::create();
            if (newScene->player)
            {
                // 直接在这里设置Farm地图的初始位置
                Vec2 tilePos = Vec2(29, 11);
                Vec2 worldPos = newScene->_gameMap->convertToWorldCoord(tilePos);
                newScene->player->setPosition(worldPos);
                newScene->player->setScale(3.0f);
                newScene->addChild(newScene->player, 1);

                // 更新相机位置
                newScene->updateCamera();
            }
        }
 
        // 确保旧场景的所有子节点被移除
        this->removeAllChildren();
       
        CCLOG("传送：准备切换场景");
        Director::getInstance()->replaceScene(
            TransitionFade::create(0.5f, newScene, Color3B(0, 0, 0))
        );
        newScene->updateCamera();
        CCLOG("传送：场景切换命令已发送");
    }
    else
    {
        CCLOG("传送失败：无法创建新场景");
    }
}



void GameScene::updateCamera()
{
    if (!player || !_gameMap) return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    // 获取地图的实际像素大小（考虑缩放）
    Size tileSize = _gameMap->getTileMap()->getTileSize();
    Size mapSize = _gameMap->getTileMap()->getMapSize();
    float scale = _gameMap->getTileMap()->getScale();
    

    // 获取玩家位置
    Vec2 playerPos = player->getPosition();

    // 计算目标视角位置（使玩家保持在屏幕中心）
    float x = std::max(playerPos.x, visibleSize.width / 2);
    float y = std::max(playerPos.y, visibleSize.height / 2);
    x = std::min(x, (mapSize.width * tileSize.width*scale - visibleSize.width/2));
    y = std::min(y, (mapSize.height * tileSize.height*scale - visibleSize.height/2));

    Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 pointB = Vec2(x, y);
    Vec2 offset = pointA - pointB;
    this->setPosition(offset);

    CCLOG("相机更新：玩家位置(%.1f, %.1f), 地图偏移(%.1f, %.1f)",
        playerPos.x, playerPos.y, offset.x, offset.y);
}
