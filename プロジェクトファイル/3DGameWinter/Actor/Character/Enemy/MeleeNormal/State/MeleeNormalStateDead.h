#pragma once
#include "Actor/Character/StateMachine.h"

class MeleeNormal;
class MeleeNormalStateDead : public StateNode<MeleeNormal>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
};

