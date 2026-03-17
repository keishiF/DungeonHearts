#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateTitle : public StateNode<Player>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

	void StartWalking();

private:
	int m_timer = 0;
	bool m_isWalk = false;
};

