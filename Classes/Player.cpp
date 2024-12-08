#include "Player.h"

USING_NS_CC;

/*
 * 创建玩家对象
 * 功能：动态创建并初始化一个新的玩家实例
 * @return 返回初始化成功的玩家对象指针，失败则返回nullptr
 */
Player* Player::create()
{
    Player* player = new (std::nothrow) Player();

    if (player && player->init())
    {
        player->autorelease();
        return player;
    }

    CC_SAFE_DELETE(player);
    return nullptr;
}

/*
 * 初始化玩家对象
 * 功能：
 * 1.加载玩家（包括行走和工具动作）精灵图片
 * 2.设置精灵显示区域（用于动画帧）
 * 3.设置精灵缩放
 * @return 初始化成功返回true，失败返回false
 */
bool Player::init()
{
    if (!Sprite::initWithFile("player.png"))
    {
        return false;
    }

    // 设置初始帧
    this->setTextureRect(cocos2d::Rect(0, 0, 48, 48));
    this->setScale(2.0f);
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

    return true;
}

/*
 * 初始化键盘监听器
 * 功能：设置键盘事件的监听，记录按键的按下和释放状态
 * 用于实时追踪玩家的键盘输入
 */
void Player::initKeyboardListener()
{
    auto keyboardListener = EventListenerKeyboard::create();
    
    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        keys[keyCode] = true;
    };
    
    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        keys[keyCode] = false;
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

/*
 * 初始化鼠标监听器
 * 功能：
 * 1.创建鼠标事件监听器
 * 2.设置鼠标点击时触发动作
 * 3.将监听器添加到事件分发器
 */
void Player::initMouseListener()
{
    auto mouseListener = EventListenerMouse::create();

    // 设置鼠标按下事件的回调函数
    mouseListener->onMouseDown = [=](Event* event)
        {
            EventMouse* e = (EventMouse*)event;
            performAction(e->getLocation());  // 在点击位置执行动作
        };

    // 将监听器添加到事件分发器，优先级与精灵相同
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

/*
 * 检查指定按键是否被按下
 * @param code 要检查的按键代码
 * @return 如果按键被按下返回true，否则返回false
 */
bool Player::isKeyPressed(EventKeyboard::KeyCode code)
{
    if (keys.find(code) != keys.end())
        return keys[code];
    return false;
}

/*
 * 设置碰撞检测组
 * @param group 地图中的碰撞对象组
 */
void Player::setCollisionGroup(cocos2d::TMXObjectGroup* group)
{
    collisionsGroup = group;
}

/*
 * 判断点是否在多边形内的辅助函数
 * 使用射线法判断点是否在多边形内部
 * @param point 待检测的点
 * @param vertices 多边形的顶点数组
 * @return 如果点在多边形内部返回true，否则返回false
 */
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

/*
 * 碰撞检测函数
 * 功能：检查玩家在给定位置是否与地图中的碰撞对象发生碰撞
 * @param nextPosition 要检查的位置坐标
 * @return 如果发生碰撞返回true，否则返回false
 */
bool Player::checkCollision(const cocos2d::Vec2& nextPosition)
{
    auto objects = collisionsGroup->getObjects();

    for (auto& obj : objects)
    {
        auto& dict = obj.asValueMap();

        if (dict.find("points") != dict.end())
        {
            float const baseX = dict["x"].asFloat() * 2;
            float const baseY = dict["y"].asFloat() * 2;

            auto points = dict["points"].asValueVector();
            std::vector<Vec2> vertices;

            for (const auto& point : points)
            {
                auto pointMap = point.asValueMap();
                float const x = pointMap["x"].asFloat() * 2 + baseX;
                float const y = -pointMap["y"].asFloat() * 2 + baseY;
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

/*
 * 设置当前的道具（暂时没用，等后续作为背包和动画的接口：点击就切换，接着可以实现对应动作）
 */
void Player::setCurrentTool(ToolType tool)
{
    currentTool = tool;
}

/*
 * 播放动作动画
 * 功能：
 * 1.切换到动作精灵
 * 2.根据手持工具种类，设置动作精灵的初始状态
 * 3.开始动作动画
 * @param clickPos 点击位置（预留参数，可用于后续扩展方向判断等）
 */
void Player::performAction(const Vec2& clickPos)
{
    // 如果没有装备工具或者动作精灵不存在，不执行动作
    if (!actionSprite || currentTool == ToolType::NONE)
    {
        return;
    }

    if (!isActioning)
    {
        isActioning = true;
        actionTimer = 0;
        currentFrame = 0;

        // 通过透明度切换精灵显示
        this->setOpacity(0);
        actionSprite->setOpacity(255);
        actionSprite->setPosition(Vec2::ZERO);

        // 根据工具类型和方向设置动作精灵的初始帧
        int toolOffset = 0;  // 工具动作的偏移量
        switch (currentTool)
        {
            case ToolType::SHOVEL:    // 铲子动作（0-3行）
                toolOffset = 0;
                break;
            case ToolType::AXE:       // 斧头动作（4-7行）
                toolOffset = 4;
                break;
            case ToolType::WATERING:  // 浇水动作（8-11行）
                toolOffset = 8;
                break;
            default:
                return;
        }

        // 计算最终的Y坐标：工具偏移 + 方向
        float y = (toolOffset + currentDirection) * 48;
        actionSprite->setTextureRect(Rect(0, y, 48, 48));
        actionSprite->setLocalZOrder(1);
    }
}

/*
 * 更新动作动画
 * 功能：
 * 1.控制动作动画的帧切换
 * 2.处理动作完成后的状态恢复
 * @param dt 两帧之间的时间间隔
 */
void Player::updateAction(float dt)
{
    if (!actionSprite || !isActioning)
    {
        return;
    }

    actionTimer += dt;
    if (actionTimer >= ACTION_DURATION / 2) // 每帧动画持续时间
    {
        actionTimer = 0;
        currentFrame++;

        if (currentFrame < 2)               // 动作素材只有两帧
        {
            // 计算工具偏移
            int toolOffset = 0;
            switch (currentTool)
            {
                case ToolType::SHOVEL:      // 铲子动作（0-3行）
                    toolOffset = 0;
                    break;
                case ToolType::AXE:         // 斧头动作（4-7行）
                    toolOffset = 4;
                    break;
                case ToolType::WATERING:    // 浇水动作（8-11行）
                    toolOffset = 8;
                    break;
                default:
                    return;
            }

            // 更新动作帧
            float x = currentFrame * 48;                        // 在图片素材中水平偏移，每帧48像素
            float y = (toolOffset + currentDirection) * 48;     // 考虑工具偏移和方向
            actionSprite->setTextureRect(Rect(x, y, 48, 48));
        }
        else  // 动作完成
        {
            // 重置状态
            isActioning = false;
            currentFrame = 0;

            // 切换回主精灵
            this->setOpacity(255);      // 主精灵显示
            actionSprite->setOpacity(0); // 动作精灵变透明
        }
    }
}

/*
 * 玩家更新函数
 * 功能：
 * 1.处理玩家的移动输入
 * 2.计算玩家的下一个位置
 * 3.进行碰撞检测
 * 4.更新玩家位置
 * @param dt 两帧之间的时间间隔
 */
void Player::update(float dt)
{
    Vec2 direction;

    // 检测移动输入
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_W) || isKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW))
        direction.y += 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_S) || isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
        direction.y -= 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_A) || isKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
        direction.x -= 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_D) || isKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
        direction.x += 1;

    if (direction != Vec2::ZERO && !isActioning)  // 只有在不执行动作时才能移动
    {
        Vec2 movement = direction.getNormalized() * moveSpeed * dt;
        Vec2 nextPosition = this->getPosition() + movement;

        if (!checkCollision(nextPosition))
        {
            this->setPosition(nextPosition);

            // 更新方向
            if (direction.y > 0)       // 上
                currentDirection = 1;
            else if (direction.y < 0)  // 下
                currentDirection = 0;
            else if (direction.x < 0)  // 左
                currentDirection = 2;
            else if (direction.x > 0)  // 右
                currentDirection = 3;

            // 更新走路动画
            animationTimer += dt;
            if (animationTimer >= FRAME_INTERVAL)
            {
                animationTimer = 0;
                currentFrame = (currentFrame + 1) % 4;
            }

            // 设置走路帧
            this->setTextureRect(Rect(
                currentFrame * 48,
                currentDirection * 48,
                48,
                48
            ));
        }
    }
    else if (!isActioning)  // 静止状态且不在执行动作
    {
        currentFrame = 0;
        this->setTextureRect(Rect(0, currentDirection * 48, 48, 48));
    }

    // 更新动作动画
    updateAction(dt);
}