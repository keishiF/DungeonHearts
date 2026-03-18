#pragma once
#include "Actor/Character/StateMachine.h"
#include "MyLib/Vector3.h"

class Boss;
class BossStateLaser : public StateNode<Boss>
{
public:
	virtual ~BossStateLaser();
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
	int m_frame = 0;
	int m_chargeEffect = -1;
	int m_playingEffect = -1;
	Vector3 m_lockedTargetPos = Vector3();
};

