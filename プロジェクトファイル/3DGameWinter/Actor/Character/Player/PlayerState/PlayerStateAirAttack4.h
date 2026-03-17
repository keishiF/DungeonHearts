#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateAirAttack4 : public StateNode<Player>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	bool m_isCombo = false;
};

