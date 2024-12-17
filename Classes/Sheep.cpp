#include "Sheep.h"
USING_NS_CC;

Sheep* Sheep::create()
{
	Sheep* sheep = new (std::nothrow) Sheep();
	if (sheep && sheep->init()) {
		sheep->autorelease();
		return sheep;
	}
	CC_SAFE_DELETE(sheep);
	return nullptr;
}

bool Sheep::init()
{
	const std::string imagePath = "Animals/Sheep.png";
	if (!NPC::init(imagePath)) {
		return false;
	}
	this->setTextureRect(cocos2d::Rect(0, 0, 32, 32));
	this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
	this->setScale(2.0);
	return true;
}

void Sheep::moveToDirection(cocos2d::Vec2& destination, float dt)
{
	cocos2d::Vec2 direction = destination - this->getPosition();
	float distance = direction.length();

	// ���·���
	if (direction.y > 0)
		currentDirection = 2;
	if (direction.y < 0)
		currentDirection = 0;
	if (direction.x < 0)
		currentDirection = 3;
	if (direction.x > 0)
		currentDirection = 1;

	// ��һ����������
	direction.normalize();
	// ����λ��
	this->setPosition(this->getPosition() + direction * moveSpeed * dt);
	// ������·����
	animationTimer += dt;
	if (animationTimer >= FRAME_INTERVAL) {
		animationTimer = 0;
		currentFrame = (currentFrame + 1) % 4;
	}
	// ������·֡
	if (currentDirection == 1) { // ����
		this->setScaleX(2.0f); // ȷ����������
		this->setTextureRect(cocos2d::Rect(currentFrame * 32, 32, 32, 32)); // ʹ�õڶ���
	}
	else if (currentDirection == 3) { // ����
		this->setScaleX(-2.0f); // ����
		this->setTextureRect(cocos2d::Rect(currentFrame * 32, 32, 32, 32)); // ʹ�õڶ���
	}
	else if (currentDirection == 2) { // ����
		this->setScaleX(2.0f); // ȷ����������
		this->setTextureRect(cocos2d::Rect(currentFrame * 32, 64, 32, 32)); // ʹ�õ�����
	}
	else if (currentDirection == 0) { // ����
		this->setScaleX(2.0f); // ȷ����������
		this->setTextureRect(cocos2d::Rect(currentFrame * 32, 0, 32, 32)); // ʹ�õ�һ��
	}

}

void Sheep::staticAnimation()
{
	currentActionState = ActionState::IDLE;
	// ��������
	auto animation = Animation::create();
	animation->setDelayPerUnit(1.0f); // ÿ֡����ʱ��

	// �������4֡����
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::create("Animals/Sheep.png", cocos2d::Rect(32 * i, 128, 32, 32)); // ÿ֡������
		animation->addSpriteFrame(frame);
	}

	// ����ѭ������
	auto runAnimation = RepeatForever::create(Animate::create(animation));
	this->runAction(runAnimation); // ���ж���
}

void Sheep::moveAlongPath(float dt)
{
	if (currentActionState != ActionState::MOVING) {
		return;
	}
	if (currentPathIndex >= path.size()) {
		currentPathIndex = 0;// �������·��ĩβ����������
	}

	cocos2d::Vec2 targetPosition = path[currentPathIndex];
	currentActionState = ActionState::MOVING;
	moveToDirection(targetPosition, dt);// �����ƶ�����

	// ����Ƿ񵽴�Ŀ��λ��
	if (this->getPosition().distance(targetPosition) < 1.0f) {
		//staticAnimation();// ���ž�ֹ����
		currentPathIndex++;// �ƶ�����һ��Ŀ��
	}
}