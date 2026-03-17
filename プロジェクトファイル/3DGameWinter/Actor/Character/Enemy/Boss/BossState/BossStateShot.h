#pragma once
#include "Actor/Character/StateMachine.h"

class Boss;
class BossStateShot : public StateNode<Boss>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_shotFrameCounter = 0;
	float m_stateDuration = 0.0f;
};

