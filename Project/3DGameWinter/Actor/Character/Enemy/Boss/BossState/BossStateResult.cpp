#include "BossStateResult.h"

#include "Actor/Character/Enemy/Boss/Boss.h"

namespace
{
    constexpr float kAnimSpeed = 34.0f;
    const char* kAnimName = "Death1";
}

void BossStateResult::OnEntry()
{
    auto boss = m_parent.lock();
    if (!boss) return;
    printf("ボス：リザルト状態エントリー\n");
    boss->GetRigidbody().SetVelo(Vector3());
    boss->GetRigidbody().SetGravity(false); 
    boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, true);
}

void BossStateResult::OnUpdate(std::shared_ptr<Camera> camera)
{
}

void BossStateResult::OnLeave()
{
}
