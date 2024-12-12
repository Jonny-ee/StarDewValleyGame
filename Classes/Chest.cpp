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

    // ���ص�һ֡��Ϊ��ʼ��ʾ�ľ���֡
    SpriteFrame* frame = SpriteFrame::create(CHEST_SPRITE_FILE,
        Rect(0, 0, FRAME_WIDTH, FRAME_HEIGHT));
    this->setSpriteFrame(frame);

    // ������������ӵ�������
    auto animation = createChestAnimation();
    AnimationCache::getInstance()->addAnimation(animation, "chest_open");

    // ���þ������ű���
    this->setScale(2.0f);

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
 * ���ܣ�Ϊ������Ӵ�����������ʵ�ֵ����������
 */
void Chest::initTouchEvents()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    // ���ô�����ʼ�¼��Ļص�����
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        Vec2 touchPos = touch->getLocation();
        Rect bounds = this->getBoundingBox();

        // ��鴥�����Ƿ��ڱ��䷶Χ���ұ���δ����
        if (bounds.containsPoint(touchPos) && !opened) {
            this->openChest();
            return true;
        }
        return false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

/*
 * �������䶯��
 * ���ܣ��Ӿ������������������
 * @return ���ش����õĶ�������
 */
Animation* Chest::createChestAnimation()
{
    Vector<SpriteFrame*> frames;

    // �Ӿ�����м������ж���֡
    for (int i = 0; i < TOTAL_FRAMES; i++)
    {
        int row = i / 5;  // ÿ��5֡
        int col = i % 5;

        auto frame = SpriteFrame::create(CHEST_SPRITE_FILE,
            Rect(col * FRAME_WIDTH, row * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT));
        frames.pushBack(frame);
    }

    return Animation::createWithSpriteFrames(frames, ANIMATION_SPEED);
}

/*
 * ��������
 * ���ܣ������俪������Ҫ�߼�
 */
void Chest::openChest()
{
    if (opened) return;

    opened = true;
    playOpenAnimation();
}

/*
 * ���ſ�������
 * ���ܣ����ű��俪���Ķ���Ч��
 */
void Chest::playOpenAnimation()
{
    auto animation = AnimationCache::getInstance()->getAnimation("chest_open");
    auto animate = Animate::create(animation);

    // �����������У����Ŷ�����Ȼ��ִ�лص�
    this->runAction(Sequence::create(
        animate,
        CallFunc::create([this]() {
            this->onOpenAnimationFinished();
            }),
        nullptr
    ));
}

/*
 * ������ɻص�
 * ���ܣ������俪��������ɺ���߼�
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
 * @param index ��Ʒ��ţ����ڴ������Ʒ����ʾʱ��
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
    float startDelay = 0.2f * index; // ��ÿ����Ʒ����ʾʱ��
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