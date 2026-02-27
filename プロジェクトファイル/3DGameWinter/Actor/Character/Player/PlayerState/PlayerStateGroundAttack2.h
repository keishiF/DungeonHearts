#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateGroundAttack2 : public StateNode<Player>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_atkSE = -1;
	bool m_isCombo = false;
};

