#include "PlayerWeapon.h"

#include "MyLib/Physics/Collider/CapsuleColliderData.h"
#include "MyLib/Physics/Collider/SphereColliderData.h"

#include <cassert>
#include <DxLib.h>

namespace
{
	// 当たり判定の半径
	constexpr float kColRadius = 75.0f;

	// モデルの回転量
	constexpr float kModelRot = DX_PI_F / 4.3f;

	// モデルの拡大値
	constexpr double kSwordModelScale = 0.01;

	// 武器をアタッチするモデルのフレーム
	constexpr int kParentModelHandFrame = 57;
}

PlayerWeapon::PlayerWeapon() :
	m_swordModel(-1),
	m_parentModel(-1),
	m_currentAttackPower(0),
	Collidable(ObjectTag::PlayerWeapon, ColliderData::Kind::Capsule)
{
}

PlayerWeapon::~PlayerWeapon()
{
	MV1DeleteModel(m_swordModel);
}

void PlayerWeapon::Init(int parentModel)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(Vector3());
	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->SetStartPos(m_rigidbody.GetPos());
	colData->SetRadius(kColRadius);

	m_parentModel = parentModel;

	m_swordModel = MV1LoadModel("Data/Model/Player/Sword.mv1");
	assert(m_swordModel >= 0);
	MV1SetScale(m_swordModel, VGet(kSwordModelScale, kSwordModelScale, kSwordModelScale));
}

void PlayerWeapon::Update(bool isAttacking, int currentFrame, int attackPower)
{
	SetActive(isAttacking);

	// プレイヤーから渡された攻撃力を保持する
	m_currentAttackPower = attackPower;

	// アタッチするモデルのMV1SetMatrixの設定を無効化する
	MV1SetMatrix(m_swordModel, MGetIdent());
	// アタッチされるモデルのフレームの行列を取得
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_parentModel, kParentModelHandFrame);
	// アタッチするモデルの拡大行列を取得
	VECTOR scale = VGet(static_cast<float>(kSwordModelScale), static_cast<float>(kSwordModelScale), static_cast<float>(kSwordModelScale));
	MATRIX scaleMat = MGetScale(scale);
	// アタッチするモデルの回転行列を取得
	MATRIX zMat = MGetRotZ(kModelRot);
	// 各行列を合成
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(zMat, mixMat);
	// 合成した行列をモデルにセット
	MV1SetMatrix(m_swordModel, mixMat);

	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->SetRadius(kColRadius);
	VECTOR start = MV1GetFramePosition(m_swordModel, 2);
	VECTOR end = MV1GetFramePosition(m_swordModel, 0);
	Vector3 startPos = { start.x, start.y, start.z };
	Vector3 endPos = { end.x, end.y, end.z };
	m_rigidbody.SetPos(endPos);
	colData->SetStartPos(startPos);
}

void PlayerWeapon::Draw()
{
	MV1DrawModel(m_swordModel);
}

void PlayerWeapon::OnCollide(std::shared_ptr<Collidable> collider)
{
}

int PlayerWeapon::GetAttackPower() const
{
	return 0;
}
