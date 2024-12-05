#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// 场景初始化
bool GameScene::init()
{
    // 初始化场景，失败返回false
    if (!Scene::init())
    {
        return false;
    }


    tileMap = TMXTiledMap::create("maps/basic.tmx");    // 加载地图文件 "basic.tmx"
    if (tileMap == nullptr)
    {
        // 如果地图加载失败，输出错误信息并返回 false
        CCLOG("Failed to load tilemap!");
        return false;
    }
    tileMap->setScale(2.0f);        // 将整个地图放大2倍，这样每个图块实际显示为32*32（素材是16*16）
    this->addChild(tileMap, 0);     // 将地图添加到场景中，层级为0

    // 创建玩家对象
    player = Player::create();
    if (player == nullptr)
    {
        return false;               // 创建失败返回false
    }

    // 设置玩家初始位置为屏幕中心
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    player->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    // 将玩家添加到场景中，层级为1
    this->addChild(player, 1);

    initKeyboardListener();     // 初始化键盘监听器
    this->scheduleUpdate();     // 启动更新调度

    // 获取地图中的碰撞层
    collisionsGroup = tileMap->getObjectGroup("collisions");
    if (collisionsGroup == nullptr)
    {
        // 如果碰撞层加载失败，输出错误信息并返回 false
        CCLOG("Failed to load collisions layer!");
        return false;
    }

    return true;                // 初始化成功
}

/*
 * 初始化键盘监听器
 * 功能：设置键盘事件的监听，记录按键的按下和释放状态
 * 用于实时追踪玩家的键盘输入
 */
void GameScene::initKeyboardListener()
{
    auto keyboardListener = EventListenerKeyboard::create();    // 创建键盘事件监听器
    
    keyboardListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        keys[keyCode] = true;                                   // 当按键被按下时，记录按键状态为true
    };
    
    keyboardListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        keys[keyCode] = false;                                  // 按键被释放时，状态为false
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);   // 将键盘监听器添加到事件分发器中，优先级为场景图
}

/*
 * 检查指定按键是否被按下
 * @param code 要检查的按键代码
 * @return 如果按键被按下返回true，否则返回false
 */
bool GameScene::isKeyPressed(EventKeyboard::KeyCode code)
{
    // 检查按键是否被按下
    if (keys.find(code) != keys.end())
        return keys[code];
    return false;
}

/*
 * 判断点是否在多边形内的辅助函数
 * 使用射线法判断点是否在多边形内部
 * @param point 待检测的点
 * @param vertices 多边形的顶点数组
 * @return 如果点在多边形内部返回true，否则返回false
 */
bool GameScene::isPointInPolygon(const Vec2& point, const std::vector<Vec2>& vertices)
{
    bool inside = false;
    int i, j = vertices.size() - 1;     // j初始化为最后一个顶点的索引

    // 遍历多边形的所有边（由相邻顶点构成）
    for (i = 0; i < vertices.size(); i++)
    {
        // 检查射线是否与多边形的边相交
        // 条件1：点的y坐标在当前边的两个顶点y坐标之间
        // 条件2：点的x坐标小于射线与边的交点的x坐标
        if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
            (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) /
                (vertices[j].y - vertices[i].y) + vertices[i].x))
        {
            inside = !inside;           // 每次相交时改变内外状态
        }
        j = i;                          // 更新前一个顶点的索引
    }

    return inside;
}

/*
 * 碰撞检测函数
 * 功能：检查玩家在给定位置是否与地图中的碰撞对象发生碰撞
 * @param nextPosition 要检查的位置坐标
 * @return 如果发生碰撞返回true，否则返回false
 */
bool GameScene::checkCollision(const cocos2d::Vec2& nextPosition)
{
    auto objects = collisionsGroup->getObjects();   // 获取地图中所有的碰撞对象

    // 遍历所有碰撞对象
    for (auto& obj : objects)
    {
        auto& dict = obj.asValueMap();

        // 检查是否为多边形对象（通过是否包含points属性判断）
        if (dict.find("points") != dict.end())
        {
            // 获取多边形的基准点坐标并应用缩放（因为整体放大了两倍）
            float const baseX = dict["x"].asFloat() * 2;
            float const baseY = dict["y"].asFloat() * 2;

            // 获取多边形的所有顶点
            auto points = dict["points"].asValueVector();
            std::vector<Vec2> vertices;

            // 转换每个顶点的坐标
            for (const auto& point : points)
            {
                auto pointMap = point.asValueMap();
                // 计算顶点的实际坐标（相对于基准点的偏移量）
                float const x = pointMap["x"].asFloat() * 2 + baseX;
                float const y = -pointMap["y"].asFloat() * 2 + baseY;   // Y坐标需要取反（Tiled和Cocos2d-x的坐标系Y轴方向相反）
                vertices.push_back(Vec2(x, y));
            }

            // 检查点是否在多边形内
            if (isPointInPolygon(nextPosition, vertices))
            {
                return true;    // 发生碰撞
            }
        }
    }
    return false;               // 没有发生碰撞
}

/*
 * 场景更新函数（移动逻辑）
 * 功能：
 * 1.处理玩家的移动输入
 * 2.计算玩家的下一个位置
 * 3.进行碰撞检测
 * 4.更新玩家位置
 * @param dt 两帧之间的时间间隔
 */
void GameScene::update(float dt)
{
    Vec2 direction;

    // 检查按键状态并更新移动方向
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_W) || isKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW))
        direction.y += 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_S) || isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
        direction.y -= 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_A) || isKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
        direction.x -= 1;
    if (isKeyPressed(EventKeyboard::KeyCode::KEY_D) || isKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
        direction.x += 1;

    // 如果按下移动按键，则进行后续移动操作
    if (direction != Vec2::ZERO)
    {
        // 计算下一个位置
        Vec2 movement = direction.getNormalized() * player->getMoveSpeed() * dt;
        Vec2 nextPosition = player->getPosition() + movement;

        // 检查是否发生碰撞
        if (!checkCollision(nextPosition))
        {
            player->setPosition(nextPosition); // 没有碰撞，则移动玩家
        }
    }
}