#include "Player.h"
#include "GameMap.h"
#include "GameScene.h"
#include "SkillUI.h"
USING_NS_CC;

Player* Player::_instance = nullptr;
const cocos2d::Vec2 Player::TOOL_ICON_POSITION = cocos2d::Vec2(100, 100);   // 工具图标位置
const cocos2d::Vec2 Player::SEED_ICON_POSITION = cocos2d::Vec2(150, 100);   // 种子图标位置

Player* Player::create()
{
    Player* player = new (std::nothrow) Player();
    if (player && player->init())
    {
        player->autorelease();
        _instance = player;  // 设置静态实例
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}

bool Player::init()
{
    if (!Sprite::initWithFile("player.png"))
    {
        return false;
    }

    // 设置初始帧
    this->setTextureRect(cocos2d::Rect(0, 0, 48, 48));
    this->setScale(3.0f);
    this->setOpacity(255);              // 确保主精灵初始时完全不透明

    // 创建动作精灵
    actionSprite = Sprite::create("actions.png");
    if (actionSprite)
    {
        actionSprite->setOpacity(0);    // 初始时完全透明
        actionSprite->setScale(1.0f);

        // 设置锚点和位置
        this->setAnchorPoint(Vec2(0.5f, 0.5f));
        actionSprite->setAnchorPoint(Vec2(0.0f, 0.0f));
        actionSprite->setPosition(Vec2::ZERO);

        this->addChild(actionSprite);
    }
    else
    {
        return false;
    }
    
    // 初始化技能UI
    skillUI = SkillUI::create();
    if (skillUI) {
        this->addChild(skillUI, 999);
        skillUI->setVisible(false);  // 初始时隐藏
        SkillSystem::getInstance()->setSkillUI(skillUI);
    }

    // 初始化十字镐精灵
    pickSprite = Sprite::create("TileSheets/Tools.png");
    if (pickSprite) {
        pickSprite->setTextureRect(cocos2d::Rect(80, 96, 16, 16));  
        pickSprite->setScale(0.8f);
        pickSprite->setPosition(Vec2(48, 0));  // 位于角色右侧
        pickSprite->setVisible(false);
        this->addChild(pickSprite);
    }

    // 初始化鱼竿精灵
    rodSprite = Sprite::create("TileSheets/Tools.png");
    if (rodSprite) {
        rodSprite->setTextureRect(cocos2d::Rect(128, 0, 16, 16));  // 第一行第九个图标
        rodSprite->setScale(1.0f);
        rodSprite->setPosition(Vec2(48, 0));  // 位于角色右侧
        rodSprite->setVisible(false);
        this->addChild(rodSprite);
    }

    // 初始化礼物精灵
    gift = Sprite::create("TileSheets/Objects_2.png");
    if (gift) {
        gift->setTextureRect(cocos2d::Rect(96, 32, 16, 16));  
        gift->setScale(1.0f);
        gift->setPosition(Vec2(48, 0));  // 位于角色右侧
        gift->setVisible(false);
        this->addChild(gift);
    }

    // 初始化胡萝卜精灵
    carrot = Sprite::create("LooseSprites/emojis.png");
    if (carrot) {
        carrot->setTextureRect(cocos2d::Rect(18, 36, 9, 9));
        carrot->setScale(1.0f);
        carrot->setPosition(Vec2(48, 0));  // 位于角色右侧
        carrot->setVisible(false);
        this->addChild(carrot);
    }
    return true;
}

void Player::moveInDirection(Vec2 direction)
{
    // 确保方向向量标准化
    direction.normalize();

    // 计算新位置
    float dt = Director::getInstance()->getDeltaTime();
    Vec2 newPos = this->getPosition() + direction * moveSpeed * dt;

    CCLOG("Player移动：方向(%.1f, %.1f), 速度%.1f, dt%.3f",
        direction.x, direction.y, moveSpeed, dt);

    // 设置新位置
    this->setPosition(newPos);
}

float Player::getMoveSpeed() const
{
    return moveSpeed;
}

void Player::switchTool()
{
    // 循环切换工具
    switch (currentTool)
    {
        case ToolType::NONE:
            currentTool = ToolType::SHOVEL;
            break;
        case ToolType::SHOVEL:
            currentTool = ToolType::AXE;
            break;
        case ToolType::AXE:
            currentTool = ToolType::PICK;
            break;
        case ToolType::PICK:
            currentTool = ToolType::WATERING;
            break;
        case ToolType::WATERING:
            currentTool = ToolType::ROD;
            break;
        case ToolType::ROD:
            currentTool = ToolType::GIFT;
            break;
        case ToolType::GIFT:
            currentTool = ToolType::CARROT;
            break;
        case ToolType::CARROT:
            currentTool = ToolType::NONE;
            break;
        default:
            currentTool = ToolType::NONE;
            break;
    }
    // 更新十字镐显示状态
    if (pickSprite)
    {
        pickSprite->setVisible(currentTool == ToolType::PICK);
    }

    // 更新鱼竿显示状态
    if (rodSprite)
    {
        rodSprite->setVisible(currentTool == ToolType::ROD);
    }

    // 更新礼物显示状态
    if (gift)
    {
        gift->setVisible(currentTool == ToolType::GIFT);
    }

    // 更新胡萝卜显示状态
    if (carrot)
    {
        carrot->setVisible(currentTool == ToolType::CARROT);
    }
}

void Player::switchSeed()
{
    switch (currentSeed)
    {
        case SeedType::CORN:
            currentSeed = SeedType::TOMATO;
            break;
        case SeedType::TOMATO:
            currentSeed = SeedType::CORN;
            break;
        default:
            currentSeed = SeedType::CORN;
            break;
    }
}

void Player::initKeyboardListener()
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
        {
            keys[keyCode] = true;

            // 对Q键的处理（切换工具）
            if (keyCode == EventKeyboard::KeyCode::KEY_Q)
            {
                switchTool();
            }
            // 对回车键的处理（显示和隐藏背包）
            if (keyCode == EventKeyboard::KeyCode::KEY_ENTER)
            {
                auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
                if (scene && scene->getInventoryUI())
                {
                    scene->getInventoryUI()->toggleVisibility();
                }
            }
            // 对K键的处理（切换技能界面）
            if (keyCode == EventKeyboard::KeyCode::KEY_K)
            {
                toggleSkillUI();
            }
            // 对E键的处理（切换种子）
            if (keyCode == EventKeyboard::KeyCode::KEY_E)
            {
                switchSeed();
            }
        };

    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event)
        {
            keys[keyCode] = false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Player::initMouseListener()
{
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [=](Event* event)
        {
            if (canPerformAction)// 如果允许执行动作
            {
                EventMouse* e = (EventMouse*)event;
                Vec2 clickPos = e->getLocation();

                performAction(clickPos);
            }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

bool Player::isKeyPressed(EventKeyboard::KeyCode code)
{
    if (keys.find(code) != keys.end())
        return keys[code];
    return false;
}

void Player::setCollisionGroup(TMXObjectGroup* group)
{
    collisionsGroup = group;
}

bool Player::isPointInPolygon(const Vec2& point, const std::vector<Vec2>& vertices)
{
    bool inside = false;
    int i, j = vertices.size() - 1;

    for (i = 0; i < vertices.size(); i++)
    {
        if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
            (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) /
                (vertices[j].y - vertices[i].y) + vertices[i].x))
        {
            inside = !inside;
        }
        j = i;
    }

    return inside;
}

bool Player::checkCollision(const Vec2& nextPosition)
{
    if (!collisionsGroup) return false;

    auto objects = collisionsGroup->getObjects();
    for (auto& obj : objects)
    {
        auto& dict = obj.asValueMap();
        if (dict.find("points") != dict.end())
        {
            float baseX = dict["x"].asFloat() * 2;
            float baseY = dict["y"].asFloat() * 2;

            auto points = dict["points"].asValueVector();
            std::vector<Vec2> vertices;

            for (const auto& point : points)
            {
                auto pointMap = point.asValueMap();
                float x = pointMap["x"].asFloat() * 2 + baseX;
                float y = -pointMap["y"].asFloat() * 2 + baseY;
                vertices.push_back(Vec2(x, y));
            }

            if (isPointInPolygon(nextPosition, vertices))
            {
                return true;
            }
        }
    }
    return false;
}

void Player::performAction(const Vec2& clickPos)
{
    if (!actionSprite || currentTool == ToolType::NONE ||
        currentTool == ToolType::ROD || currentTool == ToolType::GIFT ||
        currentTool == ToolType::CARROT)
    {
        return;
    }

    if (!isActioning)
    {
        // 如果是斧头，优先检查是否在砍树
        if (currentTool == ToolType::AXE)
        {
            auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
            if (scene)
            {
                Vec2 playerPos = this->getPosition();

                // 遍历所有树
                for (auto tree : scene->trees)
                {
                    Vec2 treePos = tree->getPosition();
                    float distance = playerPos.distance(treePos);

                    // 如果距离足够近且树可以被砍
                    if (distance < 100.0f && tree->canBeChopped())
                    {
                        // 播放砍树动画和效果
                        // 创建砍树特效
                        auto chopEffect = Sprite::create("effects/chop_effect.png");
                        if (chopEffect)
                        {
                            chopEffect->setPosition(tree->getPosition());
                            scene->addChild(chopEffect, 10);

                            auto fadeOut = FadeOut::create(0.3f);
                            auto removeEffect = RemoveSelf::create();
                            chopEffect->runAction(Sequence::create(fadeOut, removeEffect, nullptr));
                        }
                        // 砍树
                        tree->chop(10);

                        // 设置动画状态
                        isActioning = true;
                        actionTimer = 0;
                        currentFrame = 0;

                        this->setOpacity(0);
                        actionSprite->setOpacity(255);
                        actionSprite->setPosition(Vec2::ZERO);

                        float y = 4 * 48 + currentDirection * 48; // 4是斧头动作的偏移
                        actionSprite->setTextureRect(Rect(0, y, 48, 48));
                        actionSprite->setLocalZOrder(1);
                        return; // 找到并砍了树后返回
                    }
                }
            }
        }
        // 如果是十字镐，检查是否在挖矿
        else if (currentTool == ToolType::PICK) {
            auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
            if (scene)
            {
                Vec2 playerPos = this->getPosition();

                // 遍历所有矿石
                for (auto ore : scene->ores) // 假设 ores 是存储矿石的容器
                {
                    Vec2 orePos = ore->getPosition();
                    float distance = playerPos.distance(orePos);

                    // 如果距离足够近且矿石可以被挖掘
                    if (distance < 100.0f && ore->canBeDug())
                    {
                        // 播放挖矿动画和效果
                        auto digEffect = Sprite::create("effects/dig_effect.png");
                        if (digEffect)
                        {
                            digEffect->setPosition(ore->getPosition());
                            scene->addChild(digEffect, 10);

                            auto fadeOut = FadeOut::create(0.3f);
                            auto removeEffect = RemoveSelf::create();
                            digEffect->runAction(Sequence::create(fadeOut, removeEffect, nullptr));
                        }
                        // 挖掘矿石
                        ore->dig(5); // 假设每次挖掘减少5的生命值

                        // 设置动画状态
                        isActioning = true;
                        actionTimer = 0;
                        currentFrame = 0;

                        this->setOpacity(0);
                        actionSprite->setOpacity(255);
                        actionSprite->setPosition(Vec2::ZERO);

                        float y = 5 * 48 + currentDirection * 48; // 5是铲子动作的偏移
                        actionSprite->setTextureRect(Rect(0, y, 48, 48));
                        actionSprite->setLocalZOrder(1);
                        return; // 找到并挖掘了矿石后返回
                    }
                }
            }
        }
        else
        {
            // 其他工具执行普通动作
            isActioning = true;
            actionTimer = 0;
            currentFrame = 0;

            this->setOpacity(0);
            actionSprite->setOpacity(255);
            actionSprite->setPosition(Vec2::ZERO);

            int toolOffset = 0;
            switch (currentTool)
            {
                case ToolType::SHOVEL:
                    toolOffset = 0;
                    break;
                case ToolType::AXE:
                    toolOffset = 4;
                    break;
                case ToolType::WATERING:
                    toolOffset = 8;
                    break;
                default:
                    return;
            }

            float y = (toolOffset + currentDirection) * 48;
            actionSprite->setTextureRect(Rect(0, y, 48, 48));
            actionSprite->setLocalZOrder(1);
        }
    }
}

void Player::updateAction(float dt)
{
    if (!actionSprite || !isActioning)
    {
        return;
    }

    actionTimer += dt;
    if (actionTimer >= ACTION_DURATION / 2)
    {
        actionTimer = 0;
        currentFrame++;

        if (currentFrame < 2)
        {
            int toolOffset = 0;
            switch (currentTool)
            {
                case ToolType::SHOVEL:
                    toolOffset = 0;
                    break;
                case ToolType::AXE:
                    toolOffset = 4;
                    break;
                case ToolType::WATERING:
                    toolOffset = 8;
                    break;
                default:
                    return;
            }

            float x = currentFrame * 48;
            float y = (toolOffset + currentDirection) * 48;
            actionSprite->setTextureRect(Rect(x, y, 48, 48));
        }
        else
        {
            isActioning = false;
            currentFrame = 0;
            this->setOpacity(255);
            actionSprite->setOpacity(0);
        }
    }
}

void Player::removeAllListeners()
{
    _eventDispatcher->removeEventListenersForTarget(this);
}

void Player::update(float dt)
{
    Vec2 direction;

    if (isKeyPressed(EventKeyboard::KeyCode::KEY_W) || isKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW))
        direction.y += 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_S) || isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
        direction.y -= 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_A) || isKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
        direction.x -= 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_D) || isKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
        direction.x += 1;

    // 更新十字镐朝向
    if (pickSprite && pickSprite->isVisible()) {
        switch (currentDirection) {
            case 0: // 下
                pickSprite->setPosition(Vec2(37, 25));
                pickSprite->setFlippedX(false);  // 恢复正常方向
                pickSprite->setLocalZOrder(1);  // 在角色上层
                break;
            case 1: // 上
                pickSprite->setPosition(Vec2(12, 25));
                pickSprite->setFlippedX(true);   // 翻转十字镐
                pickSprite->setLocalZOrder(-1);  // 在角色下层
                break;
            case 2: // 左
                pickSprite->setPosition(Vec2(15, 25));
                pickSprite->setFlippedX(true);   // 翻转十字镐
                pickSprite->setLocalZOrder(1);  // 在角色上层
                break;
            case 3: // 右
                pickSprite->setPosition(Vec2(30, 25));
                pickSprite->setFlippedX(false);  // 恢复正常方向
                pickSprite->setLocalZOrder(-1);  // 在角色下层
                break;
        }
    }

    // 更新鱼竿朝向
    if (rodSprite && rodSprite->isVisible()) {
        switch (currentDirection) {
            case 0: // 下
                rodSprite->setPosition(Vec2(37, 25));
                rodSprite->setFlippedX(false);  // 恢复正常方向
                rodSprite->setLocalZOrder(1);  // 在角色上层
                break;
            case 1: // 上
                rodSprite->setPosition(Vec2(12, 25));
                rodSprite->setFlippedX(true);   // 翻转鱼竿
                rodSprite->setLocalZOrder(-1);  // 在角色下层
                break;
            case 2: // 左
                rodSprite->setPosition(Vec2(15, 25));
                rodSprite->setFlippedX(true);   // 翻转鱼竿
                rodSprite->setLocalZOrder(1);  // 在角色上层
                break;
            case 3: // 右
                rodSprite->setPosition(Vec2(30, 25));
                rodSprite->setFlippedX(false);  // 恢复正常方向
                rodSprite->setLocalZOrder(-1);  // 在角色下层
                break;
        }
    }

    // 更新礼物朝向
    if (gift && gift->isVisible()) {
        switch (currentDirection) {
            case 0: // 下
                gift->setPosition(Vec2(37, 25));
                gift->setFlippedX(false);  // 恢复正常方向
                gift->setLocalZOrder(1);  // 在角色上层
                break;
            case 1: // 上
                gift->setPosition(Vec2(12, 25));
                gift->setFlippedX(true);   // 翻转礼物
                gift->setLocalZOrder(-1);  // 在角色下层
                break;
            case 2: // 左
                gift->setPosition(Vec2(15, 25));
                gift->setFlippedX(true);   // 翻转礼物
                gift->setLocalZOrder(1);  // 在角色上层
                break;
            case 3: // 右
                gift->setPosition(Vec2(30, 25));
                gift->setFlippedX(false);  // 恢复正常方向
                gift->setLocalZOrder(-1);  // 在角色下层
                break;
        }
    }

    // 更新胡萝卜朝向
    if (carrot && carrot->isVisible()) {
        switch (currentDirection) {
            case 0: // 下
                carrot->setPosition(Vec2(37, 25));
                carrot->setFlippedX(false);  // 恢复正常方向
                carrot->setLocalZOrder(1);  // 在角色上层
                break;
            case 1: // 上
                carrot->setPosition(Vec2(12, 25));
                carrot->setFlippedX(true);   // 翻转胡萝卜
                carrot->setLocalZOrder(-1);  // 在角色下层
                break;
            case 2: // 左
                carrot->setPosition(Vec2(15, 25));
                carrot->setFlippedX(true);   // 翻转胡萝卜
                carrot->setLocalZOrder(1);  // 在角色上层
                break;
            case 3: // 右
                carrot->setPosition(Vec2(30, 25));
                carrot->setFlippedX(false);  // 恢复正常方向
                carrot->setLocalZOrder(-1);  // 在角色下层
                break;
        }
    }

    if (direction != Vec2::ZERO && !isActioning)
    {
        Vec2 movement = direction.getNormalized() * moveSpeed * dt;
        Vec2 nextPosition = this->getPosition() + movement;

        if (gameMap && gameMap->isWalkable(nextPosition))
        {
            this->setPosition(nextPosition);

            if (direction.y > 0)
                currentDirection = 1;
            else if (direction.y < 0)
                currentDirection = 0;
            else if (direction.x < 0)
                currentDirection = 2;
            else if (direction.x > 0)
                currentDirection = 3;

            animationTimer += dt;
            if (animationTimer >= FRAME_INTERVAL)
            {
                animationTimer = 0;
                currentFrame = (currentFrame + 1) % 4;
            }

            this->setTextureRect(Rect(
                currentFrame * 48,
                currentDirection * 48,
                48,
                48
            ));
        }
    }
    else if (!isActioning)
    {
        currentFrame = 0;
        this->setTextureRect(Rect(0, currentDirection * 48, 48, 48));
    }

    updateAction(dt);
}

void Player::toggleSkillUI() {
    if (skillUI) {
        isSkillUIVisible = !isSkillUIVisible;
        skillUI->setVisible(isSkillUIVisible);

        if (isSkillUIVisible) {
            // 显示时的动画
            skillUI->setScale(0.8f);
            skillUI->setOpacity(0);

            auto scaleAction = ScaleTo::create(0.2f, 1.0f);
            auto fadeAction = FadeIn::create(0.2f);
            skillUI->runAction(Spawn::create(scaleAction, fadeAction, nullptr));
        }
        else {
            // 隐藏时的动画
            auto scaleAction = ScaleTo::create(0.2f, 0.8f);
            auto fadeAction = FadeOut::create(0.2f);
            skillUI->runAction(Spawn::create(scaleAction, fadeAction, nullptr));
        }
    }
}

void Player::digOre(Ore* ore)
{
    ore->dig(5); // 挖掘矿石，假设每次挖掘减少5的生命值
}
