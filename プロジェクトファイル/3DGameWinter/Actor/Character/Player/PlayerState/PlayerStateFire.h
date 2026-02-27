#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateFire : public StateNode<Player>
{
public:
	PlayerStateFire();
	virtual ~PlayerStateFire();

	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_fireSE = -1;
	int m_frame = 0;
	bool m_isAttacked = false;
};

