#include "GameScene.h"

USING_NS_CC;

/*
 * 创建场景
 * 功能：创建并返回一个新的游戏场景实例
 * @return 返回创建的场景对象
 */
Scene* GameScene::createScene()
{
    return GameScene::create();
}

/*
 * 初始化场景
 * 功能：
 * 1. 加载地图
 * 2. 创建玩家
 * 3. 设置碰撞检测
 * @return 初始化成功返回true，失败返回false
 */
bool GameScene::init()
{
    // 初始化场景，失败返回false
    if (!Scene::init())
    {
        return false;
    }

    tileMap = TMXTiledMap::create("map/basic.tmx");    // 加载地图文件 "basic.tmx"
    if (tileMap == nullptr)
    {
        // 如果地图加载失败，输出错误信息并返回 false
        CCLOG("Failed to load tilemap!");
        return false;
    }
    tileMap->setScale(2.0f);        // 将整个地图放大2倍，这样每个图块实际显示为32*32（素材是16*16）
    this->addChild(tileMap, 0);     // 将地图添加到场景中，层级为0

    // 获取碰撞层
    collisionsGroup = tileMap->getObjectGroup("collisions");
    if (collisionsGroup == nullptr)
    {
        // 如果碰撞层加载失败，输出错误信息并返回 false
        CCLOG("Failed to load collisions layer!");
        return false;
    }

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

    // 设置碰撞组并初始化键盘监听
    player->setCollisionGroup(collisionsGroup);
    player->initKeyboardListener();
    player->initMouseListener();  // 在添加player到场景之后调用
    this->addChild(player, 1);
    this->scheduleUpdate();

    return true;                // 初始化成功
}

/*
 * 场景更新函数
 * 功能：更新场景中所有游戏对象的状态
 * @param dt 两帧之间的时间间隔
 */
void GameScene::update(float dt)
{
    player->update(dt);
}