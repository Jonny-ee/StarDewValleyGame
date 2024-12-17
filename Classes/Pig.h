#pragma once
#include "NPC.h"

class Pig :public NPC
{
public:
	static Pig* create();
	virtual bool init();
	void moveToDirection(cocos2d::Vec2& destination, float dt);//�ƶ���Ŀ�ĵ�
	void staticAnimation();//��ֹʱ�ĺ�������
	void moveAlongPath(float dt);//�ƶ���·��
	ActionState currentActionState = ActionState::MOVING; // ��ǰ����״̬
	std::vector<cocos2d::Vec2> path; // �ƶ�·��
	void setPath(const std::vector<cocos2d::Vec2>& newPath) {
		path = newPath;
		currentPathIndex = 0; // ����·������
	}
private:
	int currentPathIndex = 0; // ��ǰ·������
	int currentDirection = 0;                   // ��ǰ����(0:��, 1:��, 2:��, 3:��)
	int currentFrame = 0;                       // ��ǰ֡����
	float animationTimer = 0;                   // ������ʱ��
	const float FRAME_INTERVAL = 0.3f;          // ֡���ʱ��
};