#pragma once
#include "Actor/Character/StateMachine.h"

class MeleeNormal;
class MeleeNormalStateWait : public StateNode<MeleeNormal>
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
    SubState m_subState;

    int m_timer = 0;
};

