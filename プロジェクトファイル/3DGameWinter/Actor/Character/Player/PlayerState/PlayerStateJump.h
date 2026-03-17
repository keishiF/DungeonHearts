#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateJump : public StateNode<Player>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_frame = 0;
};

