#include "Chest.h"

USING_NS_CC;

/*
 * ��������ʵ���ľ�̬����
 * ���ܣ���������ʼ��һ���������
 * @return ���ر������ָ�룬��ʼ��ʧ�ܷ���nullptr
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
 * ��ʼ���������
 * ���ܣ���ʼ������Ļ������ԺͶ���
 * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
 */
bool Chest::init()
{
    if (!Sprite::init())
    {
        return false;
    }

    // ��ʼ�����俪��״̬
    opened = false;

    //���ص�һ֡��Ϊ��ʼ��ʾ�ľ�̬֡
    SpriteFrame* frame = SpriteFrame::create(CHEST_SPRITE_FILE,
        Rect(0, 0, FRAME_WIDTH, FRAME_HEIGHT));
    if (!frame) {
        CCLOG("Failed to load chest sprite frame");
        return false;
    }
    this->setSpriteFrame(frame);

    // ���������濪�����
    auto animation = createChestAnimation();
    if (animation) {
        AnimationCache::getInstance()->addAnimation(animation, "chest_open");
        CCLOG("Chest animation cached successfully");
    }

    // ���ñ�������ű���
    this->setScale(2.0f);

    // ��ʼ�������¼� 
    initTouchEvents();

    // ��ʼ������ʱ��Ϊ0
    lastOpenDay = 0;
    lastOpenMonth = 0;
    lastOpenYear = 0;

    // ���ø��µ���
    this->scheduleUpdate();

    return true;
}


/*
 * ���ñ���λ��
 * ���ܣ����ñ����ڵ�ͼ�ϵ���ʾλ��
 * @param position Ŀ��λ�õ�����
 */
void Chest::setChestPosition(const Vec2& position)
{
    this->setPosition(position);
}

/*
 * ��ʼ�������¼�
 * ���ܣ�Ϊ������Ӵ�����������ʵ�ֵ���������
 */
void Chest::initTouchEvents()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    // ��ӵ�����־
    CCLOG("Initializing chest touch events");

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        Vec2 touchPos = touch->getLocation();
        Vec2 locationInNode = this->convertToNodeSpace(touchPos);
        Size s = this->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        // ��ӵ�����־
        CCLOG("Touch at position: (%.1f, %.1f)", touchPos.x, touchPos.y);
        CCLOG("Chest bounds: (%.1f, %.1f, %.1f, %.1f)",
            rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

        // ��ȡ��ǰ����
        auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
        if (!scene) {
            CCLOG("Failed to get GameScene");
            return false;
        }

        // ��ȡ���
        auto player = scene->getPlayer();
        if (!player) {
            CCLOG("Failed to get Player");
            return false;
        }

        // ��鴥���Ƿ��ڱ��䷶Χ���ұ���δ����
        if (rect.containsPoint(locationInNode) && !opened) {
            CCLOG("Touch is within chest bounds");

            // �������Ƿ��ڽ�����Χ��
            if (isPlayerInRange(player->getPosition())) {
                CCLOG("Player is in range, opening chest");
                this->openChest();
                return true;
            }
            else {
                CCLOG("Player is too far from chest");
                // ����������UI��ʾ
            }
        }
        return false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    CCLOG("Chest touch events initialized");
}


/*
 * �������䶯��
 * ���ܣ��Ӿ������������������
 * @return ���ش����õĶ�������
 */

Animation* Chest::createChestAnimation()
{
    Vector<SpriteFrame*> frames;

    // ֻʹ���ϰ벿�ֵ�5֡��Ϊ�������
    for (int i = 0; i < 5; i++)  // �޸�Ϊֻʹ��5֡
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
 * �������
 * ���ܣ�������俪�����Ҫ�߼�
 */
void Chest::openChest()
{
    if (opened) {
        // ����Ƿ�������¿���
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

    // ����Ƿ��Ѿ���������һ��
    if (currentYear > lastOpenYear) return true;
    if (currentYear == lastOpenYear && currentMonth > lastOpenMonth) return true;
    if (currentYear == lastOpenYear && currentMonth == lastOpenMonth && currentDay > lastOpenDay) return true;

    return false;
}


/*
 * ���ſ������
 * ���ܣ����ű��俪��Ķ���Ч��
 */
void Chest::playOpenAnimation()
{
    auto animation = AnimationCache::getInstance()->getAnimation("chest_open");
    if (!animation) {
        CCLOG("Failed to get chest_open animation from cache");
        return;
    }

    auto animate = Animate::create(animation);

    // ����һ���ص��������ڶ�����ɺ��������һ֡
    auto setFinalFrame = CallFunc::create([this]() {
        // ����Ϊ����״̬�����һ֡
        this->setSpriteFrame(SpriteFrame::create(CHEST_SPRITE_FILE,
        Rect(4 * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT)));
    this->onOpenAnimationFinished();
        });

    // ���ж������У����Ŷ�����Ȼ���������һ֡
    this->runAction(Sequence::create(
        animate,
        setFinalFrame,
        nullptr
    ));
}

/*
 * ������ɻص�
 * ���ܣ�������俪�������ɺ���߼�
 * 1. ���������Ʒ
 * 2. ����Ʒ��ӵ���Ʒϵͳ��
 * 3. ��ʾ�����Ʒ��Ʈ��Ч��
 */
void Chest::onOpenAnimationFinished()
{
    // ��ȡ��Ʒϵͳʵ��
    auto itemSystem = ItemSystem::getInstance();

    // ���������Ʒ����
    int itemCount = random(MIN_ITEMS, MAX_ITEMS);

    // ���������Ʒ����ӵ���Ʒϵͳ
    for (int i = 0; i < itemCount; i++) {
        std::string randomItem = generateRandomItem();

        // �����Ʒ��ϵͳ
        if (itemSystem->addItem(randomItem, 1)) {
            // ������Ʒ�����ʾ
            createItemObtainEffect(randomItem, i);
        }

        CCLOG("Chest generated item: %s", randomItem.c_str());
    }
}

/*
 * ���������Ʒ
 * ���ܣ��ӿ��ܵ���Ʒ�б������ѡ��һ����Ʒ
 * @return ���������Ʒ��ID�ַ���
 */
std::string Chest::generateRandomItem() const
{
    int index = random(0, (int)possibleItems.size() - 1);
    return possibleItems[index];
}

/*
 * ������Ʒ���Ч��
 * ���ܣ���ʾ�����Ʒ��Ʈ�ֺ�ͼ��Ч��
 * @param itemId ��ƷID
 * @param index ��Ʒ��ţ����ڴ�������Ʒ����ʾʱ��
 */
void Chest::createItemObtainEffect(const std::string& itemId, int index)
{
    // ������Ʒͼ��
    auto itemIcon = Sprite::create("items/" + itemId + ".png");
    if (!itemIcon) return;

    // ���ó�ʼλ�ã��ӱ���λ�ÿ�ʼ��
    itemIcon->setPosition(Vec2(this->getContentSize().width / 2,
        this->getContentSize().height));
    this->addChild(itemIcon);

    // ������Ʒ���Ʊ�ǩ
    auto itemLabel = Label::createWithTTF("+" + itemId, "fonts/arial.ttf", 20);
    itemLabel->setPosition(itemIcon->getPosition() + Vec2(50, 0));
    this->addChild(itemLabel);

    // ���ó�ʼ״̬
    float startDelay = 0.2f * index; // ���ÿ����Ʒ����ʾʱ��
    itemIcon->setScale(0.0f);
    itemIcon->setOpacity(0);
    itemLabel->setOpacity(0);

    // ����ͼ�궯��
    auto iconAppear = Spawn::create(
        ScaleTo::create(0.2f, 1.0f),
        FadeIn::create(0.2f),
        nullptr
    );

    // ��������Ʈ���͵�������
    auto moveUp = MoveBy::create(1.0f, Vec2(0, 100));
    auto fadeOut = FadeOut::create(0.3f);

    // ����ͼ�궯������
    itemIcon->runAction(Sequence::create(
        DelayTime::create(startDelay),
        iconAppear,
        Spawn::create(moveUp, fadeOut, nullptr),
        RemoveSelf::create(),
        nullptr
    ));

    // ���б�ǩ��������
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


void Chest::resetChest()
{
    opened = false;

    // ���ñ������
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
