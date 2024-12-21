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

    // 初始化开启时间为0
    lastOpenDay = 0;
    lastOpenMonth = 0;
    lastOpenYear = 0;

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
    if (opened) {
        // 检查是否可以重新开启
        if (canReopen()) {
            resetChest();
        }
        else {
            return;
        }
    }

    opened = true;
    recordOpenTime();
    playOpenAnimation();
}

bool Chest::canReopen() const
{
    auto gameTime = GameTime::getInstance();
    int currentDay = gameTime->getDay();
    int currentMonth = gameTime->getMonth();
    int currentYear = gameTime->getYear();

    // 检查是否已经过了至少一天
    if (currentYear > lastOpenYear) return true;
    if (currentYear == lastOpenYear && currentMonth > lastOpenMonth) return true;
    if (currentYear == lastOpenYear && currentMonth == lastOpenMonth && currentDay > lastOpenDay) return true;

    return false;
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
    _obtainedItems.clear(); // 清空之前的物品列表

    // 生成随机物品数量
    int itemCount = random(MIN_ITEMS, MAX_ITEMS);

    // 生成随机物品并添加到物品系统
    for (int i = 0; i < itemCount; i++) {
        std::string randomItem = generateRandomItem();
        int quantity = 1;

        // 添加物品到系统
        if (itemSystem->addItem(randomItem, quantity)) {
            // 记录获得的物品
            _obtainedItems.push_back({ randomItem, quantity });
        }
        CCLOG("宝箱生成物品: %s", randomItem.c_str());
    }

    // 显示物品汇总弹窗
    showItemsSummaryPopup();

    // 添加挖矿经验
    auto skillSystem = SkillSystem::getInstance();
    if (skillSystem) {
        const int CHEST_MINING_EXP = 2;  // 每次开启宝箱获得2点挖矿经验
        skillSystem->gainExp(SkillType::MINING, CHEST_MINING_EXP);

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




void Chest::resetChest()
{
    opened = false;

    // 重置宝箱外观
    SpriteFrame* frame = SpriteFrame::create(CHEST_SPRITE_FILE,
        Rect(0, 0, FRAME_WIDTH, FRAME_HEIGHT));
    if (frame) {
        this->setSpriteFrame(frame);
    }

    CCLOG("Chest has been reset and can be opened again");
}

void Chest::recordOpenTime()
{
    auto gameTime = GameTime::getInstance();
    lastOpenDay = gameTime->getDay();
    lastOpenMonth = gameTime->getMonth();
    lastOpenYear = gameTime->getYear();

    CCLOG("Chest opened on Year %d, Month %d, Day %d",
        lastOpenYear, lastOpenMonth, lastOpenDay);
}


void Chest::showItemsSummaryPopup()
{
    if (_obtainedItems.empty()) {
        return;
    }

    // 创建半透明黑色背景
    // 根据物品数量调整背景高度
    float bgHeight = 60 + (_obtainedItems.size() * 30); // 基础高度 + 每个物品的高度
    auto popupBg = LayerColor::create(Color4B(0, 0, 0, 150), 200, bgHeight);

    // 创建标题
    auto titleLabel = Label::createWithSystemFont("Congratulations!", "Arial", 24);
    titleLabel->setPosition(Vec2(100, bgHeight - 30));
    titleLabel->setColor(Color3B::WHITE);
    popupBg->addChild(titleLabel);

    // 为每个物品创建一行文本
    for (size_t i = 0; i < _obtainedItems.size(); i++) {
        const auto& item = _obtainedItems[i];
        auto itemLabel = Label::createWithSystemFont(
            item.itemId + " x" + std::to_string(item.quantity),
            "Arial", 20);
        itemLabel->setPosition(Vec2(100, bgHeight - 60 - (i * 30)));
        itemLabel->setColor(Color3B::WHITE);
        popupBg->addChild(itemLabel);
    }

    // 设置弹窗位置在屏幕中央
    Size visibleSize = Director::getInstance()->getVisibleSize();
    popupBg->setPosition(Vec2(
        (visibleSize.width - popupBg->getContentSize().width) / 2,
        (visibleSize.height - popupBg->getContentSize().height) / 2
    ));

    // 添加到场景
    this->getParent()->addChild(popupBg, 100);

    // 创建弹窗动画
    popupBg->setScale(0);
    popupBg->runAction(Sequence::create(
        ScaleTo::create(0.2f, 1.0f),
        DelayTime::create(2.0f), // 显示时间延长到2秒
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr
    ));
}