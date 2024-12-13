#include "Chest.h"

USING_NS_CC;

/*
 * 创建宝箱实例的静态方法
 * 功能：创建并初始化一个宝箱对象
 * @return 返回宝箱对象指针，初始化失败返回nullptr
 */
Chest* Chest::create()
{
    Chest* chest = new (std::nothrow) Chest();
    if (chest && chest->init())
    {
        chest->autorelease();
        return chest;
    }
    CC_SAFE_DELETE(chest);
    return nullptr;
}

/*
 * 初始化宝箱对象
 * 功能：初始化宝箱的基本属性和动画
 * @return 初始化成功返回true，失败返回false
 */
bool Chest::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // 初始化宝箱开启状态
    opened = false;

    //加载第一帧作为初始显示的静态帧
    SpriteFrame* frame = SpriteFrame::create(CHEST_SPRITE_FILE,
        Rect(0, 0, FRAME_WIDTH, FRAME_HEIGHT));
    if (!frame) {
        CCLOG("Failed to load chest sprite frame");
        return false;
    }
    this->setSpriteFrame(frame);

    // 创建并缓存开启动画
    auto animation = createChestAnimation();
    if (animation) {
        AnimationCache::getInstance()->addAnimation(animation, "chest_open");
        CCLOG("Chest animation cached successfully");
    }

    // 设置宝箱的缩放比例
    this->setScale(2.0f);

    // 初始化触摸事件 
    initTouchEvents();

    // 启用更新调度
    this->scheduleUpdate();

    return true;
}


/*
 * 设置宝箱位置
 * 功能：设置宝箱在地图上的显示位置
 * @param position 目标位置的坐标
 */
void Chest::setChestPosition(const Vec2& position)
{
    this->setPosition(position);
}

/*
 * 初始化触摸事件
 * 功能：为宝箱添加触摸监听器，实现点击开启功能
 */
void Chest::initTouchEvents()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    // 添加调试日志
    CCLOG("Initializing chest touch events");

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        Vec2 touchPos = touch->getLocation();
        Vec2 locationInNode = this->convertToNodeSpace(touchPos);
        Size s = this->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        // 添加调试日志
        CCLOG("Touch at position: (%.1f, %.1f)", touchPos.x, touchPos.y);
        CCLOG("Chest bounds: (%.1f, %.1f, %.1f, %.1f)",
            rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

        // 获取当前场景
        auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
        if (!scene) {
            CCLOG("Failed to get GameScene");
            return false;
        }

        // 获取玩家
        auto player = scene->getPlayer();
        if (!player) {
            CCLOG("Failed to get Player");
            return false;
        }

        // 检查触摸是否在宝箱范围内且宝箱未开启
        if (rect.containsPoint(locationInNode) && !opened) {
            CCLOG("Touch is within chest bounds");

            // 检查玩家是否在交互范围内
            if (isPlayerInRange(player->getPosition())) {
                CCLOG("Player is in range, opening chest");
                this->openChest();
                return true;
            }
            else {
                CCLOG("Player is too far from chest");
                // 这里可以添加UI提示
            }
        }
        return false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    CCLOG("Chest touch events initialized");
}


/*
 * 创建宝箱动画
 * 功能：从精灵表创建开启动画序列
 * @return 返回创建好的动画对象
 */

Animation* Chest::createChestAnimation()
{
    Vector<SpriteFrame*> frames;

    // 只使用上半部分的5帧作为开启动画
    for (int i = 0; i < 5; i++)  // 修改为只使用5帧
    {
        auto frame = SpriteFrame::create(CHEST_SPRITE_FILE,
            Rect(i * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT));
        if (frame) {
            frames.pushBack(frame);
            CCLOG("Added frame %d for chest animation", i);
        }
        else {
            CCLOG("Failed to create frame %d", i);
        }
    }

    return Animation::createWithSpriteFrames(frames, ANIMATION_SPEED);
}

bool Chest::isPlayerInRange(const Vec2& playerPos) const
{
    Vec2 chestPos = this->getPosition();
    float distance = chestPos.distance(playerPos);
    return distance <= INTERACTION_RANGE;
}

/*
 * 开启宝箱
 * 功能：处理宝箱开启的主要逻辑
 */
void Chest::openChest()
{
    if (opened) return;

    opened = true;
    playOpenAnimation();
}

/*
 * 播放开启动画
 * 功能：播放宝箱开启的动画效果
 */
void Chest::playOpenAnimation()
{
    auto animation = AnimationCache::getInstance()->getAnimation("chest_open");
    if (!animation) {
        CCLOG("Failed to get chest_open animation from cache");
        return;
    }

    auto animate = Animate::create(animation);

    // 创建一个回调函数，在动画完成后设置最后一帧
    auto setFinalFrame = CallFunc::create([this]() {
        // 设置为开启状态的最后一帧
        this->setSpriteFrame(SpriteFrame::create(CHEST_SPRITE_FILE,
        Rect(4 * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT)));
    this->onOpenAnimationFinished();
        });

    // 运行动画序列：播放动画，然后设置最后一帧
    this->runAction(Sequence::create(
        animate,
        setFinalFrame,
        nullptr
    ));
}

/*
 * 动画完成回调
 * 功能：处理宝箱开启动画完成后的逻辑
 * 1. 生成随机物品
 * 2. 将物品添加到物品系统中
 * 3. 显示获得物品的飘字效果
 */
void Chest::onOpenAnimationFinished()
{
    // 获取物品系统实例
    auto itemSystem = ItemSystem::getInstance();

    // 随机生成物品数量
    int itemCount = random(MIN_ITEMS, MAX_ITEMS);

    // 生成随机物品并添加到物品系统
    for (int i = 0; i < itemCount; i++) {
        std::string randomItem = generateRandomItem();

        // 添加物品到系统
        if (itemSystem->addItem(randomItem, 1)) {
            // 创建物品获得提示
            createItemObtainEffect(randomItem, i);
        }

        CCLOG("Chest generated item: %s", randomItem.c_str());
    }
}

/*
 * 生成随机物品
 * 功能：从可能的物品列表中随机选择一个物品
 * @return 返回随机物品的ID字符串
 */
std::string Chest::generateRandomItem() const
{
    int index = random(0, (int)possibleItems.size() - 1);
    return possibleItems[index];
}

/*
 * 创建物品获得效果
 * 功能：显示获得物品的飘字和图标效果
 * @param itemId 物品ID
 * @param index 物品序号，用于错开多个物品的显示时间
 */
void Chest::createItemObtainEffect(const std::string& itemId, int index)
{
    // 创建物品图标
    auto itemIcon = Sprite::create("items/" + itemId + ".png");
    if (!itemIcon) return;

    // 设置初始位置（从宝箱位置开始）
    itemIcon->setPosition(Vec2(this->getContentSize().width / 2,
        this->getContentSize().height));
    this->addChild(itemIcon);

    // 创建物品名称标签
    auto itemLabel = Label::createWithTTF("+" + itemId, "fonts/arial.ttf", 20);
    itemLabel->setPosition(itemIcon->getPosition() + Vec2(50, 0));
    this->addChild(itemLabel);

    // 设置初始状态
    float startDelay = 0.2f * index; // 错开每个物品的显示时间
    itemIcon->setScale(0.0f);
    itemIcon->setOpacity(0);
    itemLabel->setOpacity(0);

    // 创建图标动画
    auto iconAppear = Spawn::create(
        ScaleTo::create(0.2f, 1.0f),
        FadeIn::create(0.2f),
        nullptr
    );

    // 创建向上飘动和淡出动画
    auto moveUp = MoveBy::create(1.0f, Vec2(0, 100));
    auto fadeOut = FadeOut::create(0.3f);

    // 运行图标动画序列
    itemIcon->runAction(Sequence::create(
        DelayTime::create(startDelay),
        iconAppear,
        Spawn::create(moveUp, fadeOut, nullptr),
        RemoveSelf::create(),
        nullptr
    ));

    // 运行标签动画序列
    itemLabel->runAction(Sequence::create(
        DelayTime::create(startDelay),
        FadeIn::create(0.2f),
        Spawn::create(
            MoveBy::create(1.0f, Vec2(0, 100)),
            FadeOut::create(0.3f),
            nullptr
        ),
        RemoveSelf::create(),
        nullptr
    ));
}