#pragma once
#include "Actor/Character/StateMachine.h"

class Boss;
class BossStateGetUp : public StateNode<Boss>
{
public:
	virtual ~BossStateGetUp();
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_roarEffect = -1;
	int m_playingEffect = -1;
	bool m_isRoar = false;
};

