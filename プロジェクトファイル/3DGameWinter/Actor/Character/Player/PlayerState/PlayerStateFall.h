#pragma once
#include "Actor/Character/StateMachine.h"

class Player;
class PlayerStateFall : public StateNode<Player>
{
public:
	void OnEntry() override;
	void OnUpdate(std::shared_ptr<Camera> camera) override;
	void OnLeave() override;

private:
};

