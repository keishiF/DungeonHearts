#pragma once
#include "Actor/Character/StateMachine.h"

class FlyNormal;
class FlyNormalStateWait : public StateNode<FlyNormal>
{
public:
    void OnEntry() override;
    void OnUpdate(std::shared_ptr<Camera> camera) override;
    void OnLeave() override;

private:
    enum class SubState
    {
        Idle,
        Chase
    };
    SubState m_subState = SubState::Idle;

    int m_timer = 0;
};

