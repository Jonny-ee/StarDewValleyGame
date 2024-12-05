#include "Player.h"

USING_NS_CC;

/*
 * ������Ҷ���
 * ���ܣ���������ʼ��һ���µ����ʵ��
 * @return ���س�ʼ���ɹ�����Ҷ���ָ�룬ʧ���򷵻�nullptr
 */
Player* Player::create()
{
    // ������Ҷ���
    Player* player = new (std::nothrow) Player();

    // ��ʼ���ɹ��������Զ��ͷųز�����
    if (player && player->init())
    {
        player->autorelease();
        return player;
    }

    // �������ʼ��ʧ�ܣ�ɾ�������ؿ�
    CC_SAFE_DELETE(player);
    return nullptr;
}

/*
 * ��ʼ����Ҷ���
 * ���ܣ�
 * 1.������Ҿ���ͼƬ
 * 2.���þ�����ʾ�������ڶ���֡��
 * 3.���þ�������
 * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
 */
bool Player::init()
{
    if (!Sprite::initWithFile("player.png"))    // ʹ���ز�ͼƬ��ʼ��
    {
        return false;
    }

    // ���㵥������֡�ĳߴ磨�ز���4*4��16֡��ֻҪ��1֡������ü���
    float frameWidth = this->getContentSize().width / 4;
    float frameHeight = this->getContentSize().height / 4;

    this->setTextureRect(cocos2d::Rect(0, 0, frameWidth, frameHeight)); // ������ʾ��1֡

    this->setScale(2.0f);   // ������Ŵ�2�������32*32��

    return true;
}

/*
 * �ƶ����
 * ���ܣ����ݸ��������ƶ���Ҿ���
 * @param direction �ƶ���������
 */
void Player::moveInDirection(Vec2 direction)
{
    Vec2 movement = direction.getNormalized() * moveSpeed;                                          // �����׼������ƶ�����
    this->setPosition(this->getPosition() + movement * Director::getInstance()->getDeltaTime());    // �������λ�ã�����֡ʱ�䣩
}

/*
 * ��ȡ����ƶ��ٶȣ������������moveSpeed��
 * @return ������ҵ�ǰ���ƶ��ٶ�
 */
float Player::getMoveSpeed() const
{
    return moveSpeed;
}