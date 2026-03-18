#pragma once
#include "ActorController.h"
#include "Animator/Animator.h"
#include "MyLib/Physics/Collider/CapsuleColliderData.h"
#include "MyLib/Physics/Collider/Collidable.h"
#include "MyLib/Physics/Collider/ColliderData.h"
#include "MyLib/Physics/ObjectTag.h"
#include "MyLib/Quaternion.h"
#include "Character/Status.h"
#include "MyLib/Vector3.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>
#include <vector>

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class Actor : public Collidable
{
public:
	Actor(ObjectTag tag, ColliderData::Kind colliderKind);
	~Actor();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) abstract;
	virtual void Draw() abstract;
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual void OnDamage(int atk) abstract;

	// 点滅処理の更新
	void UpdateBlinking();
	// 現在のフレームで表示すべきか
	bool IsVisible() const;

	// ゲッター
	// モデル取得
	int GetModel() const { return m_model; }
	// 各ステータスを見るためのStatusComponentを取得
	Status GetStatusComp() const { return m_status; }
	// 半径取得
	float GetRadius() const { return m_radius; }
	// 死んでいるかどうか
	bool IsDead() const { return m_isDead; }
	// 回転の取得
	Quaternion GetRotation() const { return m_rotQ; }
	// アニメーター取得
	Animator& GetAnimator() { return m_animator; }
	std::weak_ptr<ActorController> GetActorController() { return m_actorController; }

	void SetHp(float hp) { m_status.m_hp = hp; }
	void SetMaxHp(float maxHp) { m_status.m_maxHp = maxHp; }
	void SetDead(bool isDead) { m_isDead = isDead; }
	void SetRotQ(Quaternion rotQ) { m_rotQ = rotQ; }
	void SetActorController(std::weak_ptr<ActorController> actorController) { m_actorController = actorController; }

protected:
	// 点滅を開始する
	void StartBlinking(int duration = 20);

	// モデルハンドル
	int m_model;
	Status m_status;
	// 半径
	float m_radius;
	// 死んでいるかどうか
	bool m_isDead;
	// 経過フレームを測る
	int m_frame;
	// 攻撃フレーム
	int m_atkFrame;
	// 点滅フレーム
	int m_blinkFrame;
	// 正面ベクトル
	Vector3 m_forward;
	// 回転
	Quaternion m_rotQ;
	// アニメーション
	Animator m_animator;

	std::weak_ptr<ActorController> m_actorController;
};

