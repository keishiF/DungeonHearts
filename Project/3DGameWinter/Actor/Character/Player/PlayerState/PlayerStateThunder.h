#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateThunder : public StateNode<Player>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	float m_frame = 0.0f;
	bool m_isAttacked = false;
};

