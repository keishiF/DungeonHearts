#include "Camera.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/EnemyBase.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Stage/StageController.h"
#include "Actor/Stage/StageObjectBase.h"
#include "UI/Option/Options.h"
#include <algorithm>
#include <cfloat>
#include <cmath>

namespace
{
	constexpr float kLerpRate = 0.05f;
	constexpr float kRotSpeed = 0.00005f;

	constexpr float kCameraPosX = 0.0f;
	constexpr float kCameraPosY = 200.0f;
	constexpr float kCameraPosZ = -300.0f;
	constexpr float kCameraOffsetY = 75.0f;

	// 揺れの強さ
	constexpr float kShakeStrength = 0.2f;
	// 揺れる時間
	constexpr float kShakeDuration = 0.5f;
}

Camera::Camera() :
	m_pos(0.0f,0.0f,0.0f),
	m_lookAtPos(0.0f,0.0f,0.0f),
	m_fov(DX_PI_F / 2.5f),
	m_isLockOn(false),
	m_cameraRotX(0.0f),
	m_cameraRotY(0.0f),
	m_shakeStrength(0.0f),
	m_shakeDuration(0.0f),
	m_shakeTime(0.0f),
	m_shakeOffset(0.0f,0.0f,0.0f)
{
}

Camera::~Camera()
{
}

void Camera::SetCamera(std::shared_ptr<Player> player)
{
	if (!player) return;

	Vector3 p = player->GetPos();

	// 初期注視点と位置をプレイヤー基準で設定
	m_lookAtPos = { p.x, p.y + kCameraOffsetY, p.z };
	m_pos = { p.x + kCameraPosX, p.y + kCameraPosY, p.z + kCameraPosZ };

	// 回転初期化
	m_cameraRotX = 0.0f;
	m_cameraRotY = 0.0f;

	// DxLib に即時反映
	DxLib::SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z), 
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}

void Camera::Update(std::shared_ptr<Player> player, std::shared_ptr<ActorController> actorController)
{
	if (!player || !actorController) return;

	// プレイヤーの位置を取得
	Vector3 playerPos = player->GetPos();

	// カメラの注視点をプレイヤーの少し上に設定
	m_lookAtPos = { playerPos.x, playerPos.y + kCameraOffsetY, playerPos.z };

	if (player->IsLockOn() && player->GetLockedOnEnemy())
	{
		auto enemy = player->GetLockedOnEnemy();
		if (enemy)
		{
			Vector3 enemyPos = enemy->GetRigidbody().GetPos();
			Vector3 targetPos = (playerPos + enemyPos) * 0.5f;
			
			float lookAtOffsetY = kCameraOffsetY;
			if (enemy->IsBoss())
			{
				lookAtOffsetY += 225.0f;
			}
			
			m_lookAtPos = { targetPos.x, targetPos.y + lookAtOffsetY, targetPos.z };

			// プレイヤー->敵方向を向く水平回転
			Vector3 dirToEnemy = enemyPos - playerPos;
			m_cameraRotX = std::atan2(dirToEnemy.x, dirToEnemy.z);
		}
	}
	else
	{
		//右スティックの入力を取得
		int inputX =0, inputY =0;
		GetJoypadAnalogInputRight(&inputX, &inputY, DX_INPUT_PAD1);

		// Apply inversion options
		auto& opt = Options::GetInstance();
		if (opt.invertX) inputX = -inputX;
		if (opt.invertY) inputY = -inputY;

		// カメラの回転を更新
		if (inputX !=0)
		{
			m_cameraRotX += inputX * kRotSpeed;
		}
		if (inputY !=0)
		{
			m_cameraRotY += inputY * kRotSpeed;
			m_cameraRotY = std::clamp(m_cameraRotY, -DX_PI_F / 3.0f, DX_PI_F / 5.80f);
		}
	}
	
	// 初期オフセット（プレイヤーの後方、上空）
	Vector3 offset = { kCameraPosX, kCameraPosY, kCameraPosZ };

	// 回転を適用してオフセットを回す
	Quaternion yaw;
	Vector3 upAxis = { 0.0f,1.0f,0.0f };
	yaw.AngleAxis(m_cameraRotX, upAxis);

	Vector3 rightAxis = yaw * Vector3(1,0,0);
	Quaternion pitch;
	pitch.AngleAxis(m_cameraRotY, rightAxis);

	Quaternion finalRotation = pitch * yaw;
	Vector3 rotatedOffset = finalRotation * offset;

	Vector3 idealPos = playerPos + rotatedOffset;

	// カメラと注視点の間に障害物がないかチェック
	float minHitDistSq = FLT_MAX;
	Vector3 closestHitPos;
	bool isHit = false;

	auto stageObjects = actorController->GetStageController()->GetAllStageObjects();
	for (const auto& stageObj : stageObjects)
	{
		int modelHandle = stageObj->GetModel();
		if (modelHandle == -1) continue;

		MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(modelHandle, -1, m_lookAtPos.ToDxVECTOR(), idealPos.ToDxVECTOR());
		if (hitResult.HitFlag)
		{
			isHit = true;
			Vector3 hitPos(hitResult.HitPosition.x, hitResult.HitPosition.y, hitResult.HitPosition.z);
			float distSq = (hitPos - m_lookAtPos).SqrLength();
			if (distSq < minHitDistSq)
			{
				minHitDistSq = distSq;
				closestHitPos = hitPos;
			}
		}
	}
	
	m_pos = isHit ? closestHitPos + (m_lookAtPos - idealPos).GetNormalize() * 10.0f : idealPos;
	
	// DxLibにカメラ位置と注視点を設定
	DxLib::SetCameraPositionAndTarget_UpVecY(VGet(m_pos.x, m_pos.y, m_pos.z), VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}

Vector3 Camera::GetForward() const
{
	Vector3 forward;
	forward.x = sinf(m_cameraRotX);
	forward.y = 0.0f;
	forward.z = cosf(m_cameraRotX);
	return forward.GetNormalize();
}

Vector3 Camera::GetRight() const
{
	Vector3 right;
	right.x = cosf(m_cameraRotX);
	right.y = 0.0f;
	right.z = -sinf(m_cameraRotX);
	return right.GetNormalize();
}
