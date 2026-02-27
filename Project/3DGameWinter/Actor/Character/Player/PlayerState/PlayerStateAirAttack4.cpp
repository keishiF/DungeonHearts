#include "PlayerStateAirAttack4.h"

#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "PlayerStateAirAttack1.h"
#include "PlayerStateRun.h"
#include "PlayerStateFall.h"

namespace
{
	// アニメーションの再生速度
	constexpr float kAirAtk4AnimSpeed = 1.75f;

	// アニメーション名
	const char* kAirAtk4AnimName = "Player|AirAttack4";
}

void PlayerStateAirAttack4::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：空中攻撃4段目状態エントリー\n");
	player->GetRigidbody().SetGravity(false);
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kAirAtk4AnimName, kAirAtk4AnimSpeed, false);
}

void PlayerStateAirAttack4::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;
	player->GetWeapon()->Update(true, 0, 10);

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
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateAirAttack4>());
		}
		else if (input.IsTrigger("Run"))
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateRun>());
		}
		else
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateFall>());
		}
		m_isCombo = false;
		return;
	}
}

void PlayerStateAirAttack4::OnLeave()
{
	auto player = m_parent.lock();
	if (!player) return;
	player->GetRigidbody().SetGravity(true);
	printf("プレイヤー：空中攻撃4段目状態リーブ\n");
}
