#pragma once
#include "Actor/Character/StateMachine.h"

class Boss;
class BossStateDeath : public StateNode<Boss>
{
public:
	virtual ~BossStateDeath();
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_deadEffect    = -1;
	int m_playingEffect = -1;
};

