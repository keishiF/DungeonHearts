#include "PlayerStateLand.h"

#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "MyLib/Input.h"

namespace
{
    // アニメーション再生速度
    constexpr float kLandAnimSpeed = 1.5f;
    // アニメーション名
    const char* kLandAnimName = "Player|JumpEnd";
}

void PlayerStateLand::OnEntry()
{
    auto player = m_parent.lock();
    if (!player) return;
    printf("プレイヤー:着地状態エントリー\n");
    player->GetRigidbody().SetVelo(Vector3());
    player->GetAnimator().ChangeAnim(kLandAnimName, kLandAnimSpeed, false);
}

void PlayerStateLand::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto player = m_parent.lock();
    if (!player) return;

    player->GetWeapon()->Update(false, 0, 0);
    
    const float currentFrame = m_parent.lock()->GetAnimator().GetCurrentFrame();
    const float totalTime = m_parent.lock()->GetAnimator().GetAnimTransitionFrame();
    if (currentFrame >= totalTime)
    {
        auto& input = Input::GetInstance();
        // 移動入力があれば走行状態へ
        if (input.IsPress("Up") || input.IsPress("Right") ||
            input.IsPress("Down") || input.IsPress("Left"))
        {
            m_machine.lock()->ChangeState(std::make_shared<PlayerStateRun>());
        }
        // なければ待機状態へ
        else
        {
            m_machine.lock()->ChangeState(std::make_shared<PlayerStateIdle>());
        }
    }
}

void PlayerStateLand::OnLeave()
{
    printf("プレイヤー:着地状態リーブ\n");
}
