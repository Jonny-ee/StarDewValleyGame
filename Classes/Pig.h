#pragma once
#include "NPC.h"

class Pig :public NPC
{
public:
	static Pig* create();
	virtual bool init();
	void moveToDirection(cocos2d::Vec2& destination, float dt);//移动到目的地
	void staticAnimation();//静止时的呼吸动画
	void moveAlongPath(float dt);//移动沿路径
	ActionState currentActionState = ActionState::MOVING; // 当前动作状态
	std::vector<cocos2d::Vec2> path; // 移动路径
	void setPath(const std::vector<cocos2d::Vec2>& newPath) {
		path = newPath;
		currentPathIndex = 0; // 重置路径索引
	}
private:
	int currentPathIndex = 0; // 当前路径索引
	int currentDirection = 0;                   // 当前朝向(0:下, 1:上, 2:左, 3:右)
	int currentFrame = 0;                       // 当前帧索引
	float animationTimer = 0;                   // 动画计时器
	const float FRAME_INTERVAL = 0.3f;          // 帧间隔时间
};