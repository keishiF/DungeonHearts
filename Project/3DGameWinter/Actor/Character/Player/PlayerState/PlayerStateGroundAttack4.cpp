#include "PlayerStateGroundAttack4.h"

#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "PlayerStateFire.h"
#include "PlayerStateGroundAttack1.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateThunder.h"

namespace
{
	// アニメーションの再生速度
	constexpr float kGroundAtk4AnimSpeed = 1.75f;

	// アニメーション名
	const char* kGroundAtk4AnimName = "Player|GroundAttack4";
}

void PlayerStateGroundAttack4::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：地上攻撃4段目状態エントリー\n");
	player->GetAnimator().ChangeAnim(kGroundAtk4AnimName, kGroundAtk4AnimSpeed, false);
}

void PlayerStateGroundAttack4::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;

	// アニメーションの現在フレームを取得
	const float currentAnimFrame = player->GetAnimator().GetCurrentFrame();

	// 攻撃判定が有効なフレーム範囲
	constexpr float kAttackStartFrame = 56.0f;
	constexpr float kAttackEndFrame = 72.0f;

	// 武器の当たり判定を有効化/無効化
	bool isWeaponActive = (currentAnimFrame >= kAttackStartFrame && currentAnimFrame <= kAttackEndFrame);
	player->GetWeapon()->Update(isWeaponActive, 0, 10);

	auto& input = Input::GetInstance();
	if (input.IsTrigger("Attack"))
	{
		m_isCombo = true;
		return;
	}

	const float currentFrame = m_parent.lock()->GetAnimator().GetCurrentFrame();
	const float totalTime = m_parent.lock()->GetAnimator().GetAnimTransitionFrame();
	if (currentFrame >= totalTime)
	{
		if (m_isCombo)
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateGroundAttack1>());
		}
		else if (input.IsPress("Up")   || input.IsPress("Right") ||
				 input.IsPress("Down") || input.IsPress("Left"))
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateRun>());
		}
		else
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateIdle>());
		}
		m_isCombo = false;
		return;
	}
}

void PlayerStateGroundAttack4::OnLeave()
{
	printf("プレイヤー：地上攻撃4段目状態リーブ\n");
}
