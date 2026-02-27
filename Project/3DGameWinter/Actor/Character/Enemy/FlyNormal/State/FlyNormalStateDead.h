#pragma once
#include "Actor/Character/StateMachine.h"

class FlyNormal;
class FlyNormalStateDead : public StateNode<FlyNormal>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
};

