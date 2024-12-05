#include "Player.h"

USING_NS_CC;

/*
 * 创建玩家对象
 * 功能：创建并初始化一个新的玩家实例
 * @return 返回初始化成功的玩家对象指针，失败则返回nullptr
 */
Player* Player::create()
{
    // 创建玩家对象
    Player* player = new (std::nothrow) Player();

    // 初始化成功，加入自动释放池并返回
    if (player && player->init())
    {
        player->autorelease();
        return player;
    }

    // 创建或初始化失败，删除并返回空
    CC_SAFE_DELETE(player);
    return nullptr;
}

/*
 * 初始化玩家对象
 * 功能：
 * 1.加载玩家精灵图片
 * 2.设置精灵显示区域（用于动画帧）
 * 3.设置精灵缩放
 * @return 初始化成功返回true，失败返回false
 */
bool Player::init()
{
    if (!Sprite::initWithFile("player.png"))    // 使用素材图片初始化
    {
        return false;
    }

    // 计算单个动画帧的尺寸（素材是4*4的16帧，只要第1帧，故需裁剪）
    float frameWidth = this->getContentSize().width / 4;
    float frameHeight = this->getContentSize().height / 4;

    this->setTextureRect(cocos2d::Rect(0, 0, frameWidth, frameHeight)); // 设置显示第1帧

    this->setScale(2.0f);   // 将人物放大2倍（变成32*32）

    return true;
}

/*
 * 移动玩家
 * 功能：根据给定方向移动玩家精灵
 * @param direction 移动方向向量
 */
void Player::moveInDirection(Vec2 direction)
{
    Vec2 movement = direction.getNormalized() * moveSpeed;                                          // 计算标准化后的移动向量
    this->setPosition(this->getPosition() + movement * Director::getInstance()->getDeltaTime());    // 更新玩家位置（考虑帧时间）
}

/*
 * 获取玩家移动速度（便于类外访问moveSpeed）
 * @return 返回玩家当前的移动速度
 */
float Player::getMoveSpeed() const
{
    return moveSpeed;
}