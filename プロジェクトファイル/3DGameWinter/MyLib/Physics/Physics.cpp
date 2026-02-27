#include "Physics.h"

#include "Collider/CapsuleColliderData.h"
#include "Collider/ColliderData.h"
#include "Collider/PolygonColliderData.h"
#include "Collider/SphereColliderData.h"
#include "MyLib/HitStopManager.h" // Add this include
#include <algorithm>
#include <cfloat>
#include <DxLib.h>
#include <EffekseerForDxLib.h>

namespace
{
	constexpr float kOverlapGap = 0.0f;

	const Vector3 kGravity = { 0.0f, -0.8f, 0.0f };
}

Physics::Physics() :
	m_hitEffect(-1),
	m_playingEffect(-1)
{
	m_hitEffect = LoadEffekseerEffect("Data/Effect/Hit.efkefc", 100.0f);
	assert(m_hitEffect >= 0);
}

Physics& Physics::GetInstance()
{
    // TODO: return ステートメントをここに挿入します
    static Physics instance;
    return instance;
}

void Physics::Entry(std::shared_ptr<Collidable> collider)
{
	// 登録
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collider) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(collider);
	}
	// 既に登録されてたらエラー
	else
	{
		assert(0 && "指定のcollidableは登録済です。");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> collider)
{
	// 登録解除
	auto count = std::erase_if(m_collidables, [collider](std::shared_ptr<Collidable> target) { return target == collider; });
	// 登録されてなかったらエラー
	if (count <= 0)
	{
		assert(0 && "指定のcollidableが登録されていません。");
	}
}

void Physics::Update()
{
	//仮処理
	for (auto& item : m_collidables)
	{
		auto& rb = item->GetRigidbody();
		rb.SetOnGround(false);
		
		if (rb.UseGravity())
		{
			rb.SetVelo(rb.GetVelo() + kGravity);
		}

		auto pos = item->m_rigidbody.GetPos();
		auto nextPos = pos + item->m_rigidbody.GetVelo();
		item->m_nextPos = nextPos;
	}

	std::vector<OnCollideInfo> onCollideInfos = CheckCollide();
	FixPosition();

	for (auto& item : onCollideInfos)
	{
		ObjectTag tagA = item.owner->GetTag();
		ObjectTag tagB = item.collider->GetTag();

		if (ShouldCallOnCollide(tagA, tagB))
		{
			item.owner->OnCollide(item.collider);

			// Trigger hit stop if player weapon hits enemy/boss
			if ((tagA == ObjectTag::PlayerWeapon && (tagB == ObjectTag::Enemy || tagB == ObjectTag::Boss)) ||
				(tagB == ObjectTag::PlayerWeapon && (tagA == ObjectTag::Enemy || tagA == ObjectTag::Boss)))
			{
				HitStopManager::GetInstance().RequestHitStop(2); // 5 frames of hit stop
			}

			if (m_playingEffect == -1)
			{
				m_playingEffect = PlayEffekseer3DEffect(m_hitEffect);
			}
			SetPosPlayingEffekseer3DEffect(m_playingEffect,
				item.owner->m_rigidbody.GetPos().ToDxVECTOR().x,
				item.owner->m_rigidbody.GetPos().ToDxVECTOR().y,
				item.owner->m_rigidbody.GetPos().ToDxVECTOR().z);
			if (IsEffekseer3DEffectPlaying(m_playingEffect))
			{
				m_playingEffect = -1;
			}
		}
	}

	// 今フレームで衝突判定を行ったポリゴンコライダーのデータを解放する
	for (auto& item : m_collidables)
	{
		if (item->GetCollData()->GetKind() == ColliderData::Kind::Polygon)
		{
			auto polygonData =
			std::static_pointer_cast<PolygonColliderData>(item->GetCollData());
			polygonData->TerminateHitDim();
		}
	}
}

void Physics::DebugDraw()
{
	//当たり判定の描画を行う
	for (auto& item : m_collidables)
	{
		// 当たり判定がオフになっているものは無視する
		if (!item->IsActive()) continue;

		if (item->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
		{
			auto capsuleData = std::static_pointer_cast<CapsuleColliderData>(item->m_colliderData);

			DrawCapsule3D(capsuleData->GetStartPos().ToDxVECTOR(), item->m_rigidbody.GetPos().ToDxVECTOR(),
				capsuleData->GetRadius(), 16, 0xff00ff, 0xff00ff, false);
		}
		else if (item->m_colliderData->GetKind() == ColliderData::Kind::Sphere)
		{
			auto sphereData = std::static_pointer_cast<SphereColliderData>(item->m_colliderData);

			DrawSphere3D(item->m_rigidbody.GetPos().ToDxVECTOR(),
				sphereData->GetRadius(), 16, 0xff00ff, 0xff00ff, false);
		}
	}
}

std::vector<Physics::OnCollideInfo> Physics::CheckCollide() const
{
	//当たっているものを入れる配列
	std::vector<OnCollideInfo> onCollideInfo;
	for (auto it1 = m_collidables.begin(); it1 != m_collidables.end(); ++it1)
	{
		auto it2 = it1;
		++it2;
		for (; it2 != m_collidables.end(); ++it2)
		{
			auto& collA = *it1;
			auto& collB = *it2;			

			//当たり判定チェック
			if (IsCollide(collA, collB))
			{
				if (!SkipFixPos(collA, collB))
				{
					FixNextPosition(collA, collB);
				}

				onCollideInfo.push_back({ collA, collB });
				onCollideInfo.push_back({ collB, collA });
			}
		}
	}
	return onCollideInfo;
}

bool Physics::IsCollide(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const
{
	// 当たり判定がオフになっているものは無視する
	if (!collA->IsActive() || !collB->IsActive()) return false;

	// 特定のタグ同士では当たり判定を取らずにスキップ
	if (SkipCheckCollide(collA, collB)) return false;

	//第一の当たり判定と第二の当たり判定がおなじものでなければ
	if (collA != collB)
	{
		//当たり判定の種類を取得
		ColliderData::Kind collAKind = collA->m_colliderData->GetKind();
		ColliderData::Kind collBKind = collB->m_colliderData->GetKind();

		//球どうしの当たり判定
		if (collAKind == ColliderData::Kind::Sphere && collBKind == ColliderData::Kind::Sphere)
		{
			return IsCollideSS(collA, collB);
		}
		//カプセル同士の当たり判定
		else if (collAKind == ColliderData::Kind::Capsule && collBKind == ColliderData::Kind::Capsule)
		{
			return IsCollideCC(collA, collB);
		}
		//球とカプセルの当たり判定
		else if (collAKind == ColliderData::Kind::Sphere && collBKind == ColliderData::Kind::Capsule ||
			collAKind == ColliderData::Kind::Capsule && collBKind == ColliderData::Kind::Sphere)
		{
			return IsCollideCS(collA, collB);
		}
		// カプセルとポリゴンの当たり判定
		else if (collAKind == ColliderData::Kind::Polygon && collBKind == ColliderData::Kind::Capsule ||
			collAKind == ColliderData::Kind::Capsule && collBKind == ColliderData::Kind::Polygon)
		{
			return IsCollideCP(collA, collB);
		}
	}
	return false;
}

bool Physics::IsCollideSS(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	//当たり判定を球にダウンキャストする
	auto colliderA = std::static_pointer_cast<SphereColliderData>(collA->m_colliderData);
	auto colliderB = std::static_pointer_cast<SphereColliderData>(collB->m_colliderData);

	//当たり判定の距離を求める
	float distance = (collA->m_nextPos - collB->m_nextPos).Length();

	//球の大きさを合わせたものよりも距離が短ければぶつかっている
	if (distance < colliderA->GetRadius() + colliderB->GetRadius())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Physics::IsCollideCC(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	//当たり判定をカプセルにダウンキャストする
	auto colliderA = std::static_pointer_cast<CapsuleColliderData>(collA->m_colliderData);
	auto colliderB = std::static_pointer_cast<CapsuleColliderData>(collB->m_colliderData);

	//カプセル同士の最短距離
	float distance = Segment_Segment_MinLength(collA->m_nextPos.ToDxVECTOR(), colliderA->GetStartPos().ToDxVECTOR(),
		collB->m_nextPos.ToDxVECTOR(), colliderB->GetStartPos().ToDxVECTOR());

	if (distance < colliderA->GetRadius() + colliderB->GetRadius())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Physics::IsCollideCS(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	//球とカプセルのコライダーデータ作成
	std::shared_ptr<ColliderData> sphereCollData;
	std::shared_ptr<ColliderData> capsuleCollData;
	float distance;
	//どちらがカプセルなのか球なのか判別してからデータを入れる
	if (collA->m_colliderData->GetKind() == ColliderData::Kind::Sphere)
	{
		sphereCollData = collA->m_colliderData;
		capsuleCollData = collB->m_colliderData;

		//カプセルの線分のデータを使うためにダウンキャスト
		auto capsuleCollider = std::static_pointer_cast<CapsuleColliderData>(capsuleCollData);
		//線分と点の最近距離を求める
		distance = Segment_Point_MinLength(collB->m_nextPos.ToDxVECTOR(),
			capsuleCollider->GetStartPos().ToDxVECTOR(), collA->m_nextPos.ToDxVECTOR());
	}
	else
	{
		capsuleCollData = collA->m_colliderData;
		sphereCollData = collB->m_colliderData;
		//カプセルの線分のデータを使うためにダウンキャスト
		auto capsuleCollider = std::static_pointer_cast<CapsuleColliderData>(capsuleCollData);
		//線分と点の最近距離を求める
		distance = Segment_Point_MinLength(collA->m_nextPos.ToDxVECTOR(),
			capsuleCollider->GetStartPos().ToDxVECTOR(), collB->m_nextPos.ToDxVECTOR());
	}
	//ダウンキャスト
	auto sphereColl = std::static_pointer_cast<SphereColliderData>(sphereCollData);
	auto capsuleColl = std::static_pointer_cast<CapsuleColliderData>(capsuleCollData);

	//円とカプセルの半径よりも円とカプセルの距離が近ければ当たっている
	if (distance < sphereColl->GetRadius() + capsuleColl->GetRadius())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Physics::IsCollideCP(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	std::shared_ptr<Collidable> capsuleCollData;
	std::shared_ptr<Collidable> polygonCollData;
	if (collA->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
	{
		capsuleCollData = collA;
		polygonCollData = collB;
	}
	else
	{
		polygonCollData = collA;
		capsuleCollData = collB;
	}

	//コライダーデータの取得
	auto capsuleColldier = std::static_pointer_cast<CapsuleColliderData>(capsuleCollData->m_colliderData);
	auto polygonCollider = std::static_pointer_cast<PolygonColliderData>(polygonCollData->m_colliderData);
	//リジッドボディ
	auto rbA = collA->m_rigidbody;

	//当たってるポリゴンの数
	MV1_COLL_RESULT_POLY_DIM hitDim = {};
	hitDim = MV1CollCheck_Capsule(
		polygonCollider->GetModelHandle(),
		-1,
		rbA.GetNextPos().ToDxVECTOR(),
		capsuleColldier->GetNextStartPos(rbA.GetVelo()).ToDxVECTOR(),
		capsuleColldier->GetRadius(),
		-1);

	//当たっていないならfalse
	if (hitDim.HitNum <= 0)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
		return false;
	}
	//当たり判定に使うので保存
	polygonCollider->SetHitDim(hitDim);

	return true;
}

void Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		Vector3 toFixedPos = item->m_nextPos - item->m_rigidbody.GetPos();
		Vector3 nextPos = item->m_rigidbody.GetPos() + toFixedPos;

		item->m_rigidbody.SetVelo(toFixedPos);
		// 位置確定
		item->m_rigidbody.SetPos(nextPos);

		//当たり判定がカプセルだったら
		if (item->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
		{
			auto capsuleCol = std::static_pointer_cast<CapsuleColliderData>(item->m_colliderData);
			//伸びるカプセルかどうか取得する
			if (!capsuleCol->m_isStartPos)
			{
				//伸びないカプセルだったら初期位置を一緒に動かす
				capsuleCol->GetStartPos()  = item->m_nextPos;
			}
		}
	}
}

void Physics::FixNextPosition(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const
{
	// 当たり判定がオフになっているものは無視する
	if (!collA->IsActive() || !collB->IsActive()) return;

	auto collAKind= collA->m_colliderData->GetKind();
	auto collBKind= collB->m_colliderData->GetKind();

	// 球同士の位置補正
	if (collAKind== ColliderData::Kind::Sphere && collBKind == ColliderData::Kind::Sphere)
	{
		FixNextPositionSS(collA, collB);
	}
	// カプセルとカプセルの位置補正
	else if (collAKind == ColliderData::Kind::Capsule && collBKind == ColliderData::Kind::Capsule)
	{
		FixNextPositionCC(collA, collB);
	}
	// カプセルとポリゴンの位置補正
	else if (collAKind == ColliderData::Kind::Polygon && collBKind == ColliderData::Kind::Capsule ||
			 collAKind == ColliderData::Kind::Capsule && collBKind == ColliderData::Kind::Polygon)
	{
		FixNextPositionCP(collA, collB);
	}
}

void Physics::FixNextPositionSS(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	Vector3 collAToCollBVec = collB->m_nextPos - collA->m_nextPos;
	collAToCollBVec.Normalize();

	auto collAData = static_pointer_cast<SphereColliderData>(collA->m_colliderData);
	auto collBData = static_pointer_cast<SphereColliderData>(collB->m_colliderData);
	float awayDist = collAData->GetRadius() + collBData->GetRadius();
	Vector3 collAToNewCollBNextPos= collAToCollBVec * awayDist;
	Vector3 fixedPos = collA->GetNextPos() + collAToNewCollBNextPos;
	collB->m_nextPos = fixedPos;
}

void Physics::FixNextPositionCC(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	auto collAData = static_pointer_cast<CapsuleColliderData>(collA->m_colliderData);
	auto collBData = static_pointer_cast<CapsuleColliderData>(collB->m_colliderData);

	Vector3 collAStartPos = collA->m_nextPos;
	Vector3 collAEndPos   = collAData->GetStartPos();

	Vector3 collBStartPos = collB->m_nextPos;
	Vector3 collBEndPos   = collBData->GetStartPos();

	// 線分同士の最近接点を求める
	Vector3 closestPointA, closestPointB;
	SegmentClosestPoint(collAStartPos, collAEndPos, collBStartPos, collBEndPos, &closestPointA, &closestPointB);

	// 最近接点間の距離と方向
	Vector3 closestDir = closestPointB - closestPointA;
	float dist = closestDir.Length();

	float awayDist = collAData->GetRadius() + collBData->GetRadius();

	if (dist == 0.0f)
	{
		closestDir = { 1.0f, 0.0f, 0.0f };
		dist = 0.000001f;
	}
	else
	{
		closestDir.Normalize();
	}

	if (dist < awayDist)
	{
		float overlap = awayDist - dist;
		Vector3 pushVector = closestDir * (overlap + kOverlapGap);
		
		// 両方のオブジェクトに半分の押し戻しを適用
		// Y軸方向の押し戻しは行わない
		Vector3 pushA = -pushVector * 0.5f;
		pushA.y = 0.0f;
		collA->m_nextPos += pushA;
		
		Vector3 pushB = pushVector * 0.5f;
		pushB.y = 0.0f;
		collB->m_nextPos += pushB;
	}
}

void Physics::FixNextPositionCP(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const
{
	std::shared_ptr<Collidable> capsuleObject; // カプセルを持つオブジェクト(Collidable)
	std::shared_ptr<Collidable> polygonObject; // ポリゴンを持つオブジェクト(Collidable)

	if (collA->GetCollData()->GetKind() == ColliderData::Kind::Capsule)
	{
		capsuleObject = collA;
		polygonObject = collB;
	}
	else
	{
		capsuleObject = collB;
		polygonObject = collA;
	}

	// オブジェクトから形状データを取得
	auto capsuleShapeData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleObject->GetCollData());
	auto polygonShapeData = std::dynamic_pointer_cast<PolygonColliderData>(polygonObject->GetCollData());
	auto& hitDim = polygonShapeData->GetHitDim();

	/*if (hitDim.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(hitDim);
		return;
	}*/

	ObjectTag tag = polygonObject->GetTag();

	// 壁との衝突処理
	if (tag == ObjectTag::Wall)
	{
		FixNextPositionCapsuleWall(capsuleObject, polygonObject);
	}
	// 床との衝突処理
	else if (tag == ObjectTag::Floor)
	{
		FixNextPositionCapsuleFloor(capsuleObject, polygonObject);
	}
	// 坂道との衝突処理
	else if (tag == ObjectTag::Slope)
	{
		FixNextPositionCapsuleSlope(capsuleObject, polygonObject);
	}
}

void Physics::FixNextPositionCapsuleWall(std::shared_ptr<Collidable>& capsuleObject, std::shared_ptr<Collidable>& polygonObject) const
{
	// オブジェクトから形状データを取得
	auto capsuleShapeData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleObject->GetCollData());
	auto polygonShapeData = std::dynamic_pointer_cast<PolygonColliderData>(polygonObject->GetCollData());
	auto& hitDim = polygonShapeData->GetHitDim();

	if (hitDim.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(hitDim);
		return;
	}

	float highestY = -FLT_MAX;
	for (int i = 0; i < hitDim.HitNum; ++i)
	{
		// ポリゴンが上面かどうかを法線で判定
		if (hitDim.Dim[i].Normal.y > 0.7f)
		{
			highestY = max(
				max(hitDim.Dim[i].Position[0].y, hitDim.Dim[i].Position[1].y),
				max(hitDim.Dim[i].Position[2].y, highestY)
			);
		}
	}

	if (highestY > -FLT_MAX)
	{
		Vector3 nextPos = capsuleObject->m_nextPos;

		// カプセルの足元が上面より下で、かつ落下中の場合
		if (nextPos.y < highestY && capsuleObject->GetRigidbody().GetVelo().y < 0.0f)
		{
			nextPos.y = highestY;
			capsuleObject->m_nextPos = nextPos;

			auto& rb = capsuleObject->GetRigidbody();
			Vector3 velo = rb.GetVelo();
			velo.y = 0.0f;
			rb.SetVelo(velo);
			rb.SetOnGround(true);
		}
	}

	Vector3 totalPushBackVec(0.0f, 0.0f, 0.0f);
	int hitCount = 0;

	// カプセルの頭と足の座標
	Vector3 headPos = capsuleShapeData->GetNextStartPos(capsuleObject->GetRigidbody().GetVelo());
	// Y座標が更新されている可能性のあるm_nextPosを足の位置として使用
	Vector3 legPos = capsuleObject->m_nextPos;

	for (int i = 0; i < hitDim.HitNum; ++i)
	{
		// ポリゴンが側面かどうかを法線で判定
		if (std::abs(hitDim.Dim[i].Normal.y) < 0.5f)
		{
			VECTOR p0 = hitDim.Dim[i].Position[0];
			VECTOR p1 = hitDim.Dim[i].Position[1];
			VECTOR p2 = hitDim.Dim[i].Position[2];

			float distSq = Segment_Triangle_MinLength_Square(headPos.ToDxVECTOR(), legPos.ToDxVECTOR(), p0, p1, p2);
			float dist = sqrtf(distSq);

			float overlap = capsuleShapeData->GetRadius() - dist;
			if (overlap > 0.0f)
			{
				Vector3 normal(hitDim.Dim[i].Normal.x, hitDim.Dim[i].Normal.y, hitDim.Dim[i].Normal.z);
				normal.y = 0.0f; // 水平方向にのみ押し出す
				normal.Normalize();

				totalPushBackVec += normal * (overlap + kOverlapGap);
				hitCount++;
			}
		}
	}

	if (hitCount > 0)
	{
		capsuleObject->m_nextPos += totalPushBackVec / static_cast<float>(hitCount);
	}

	// 結果の解放
	//MV1CollResultPolyDimTerminate(hitDim);
}

void Physics::FixNextPositionCapsuleFloor(std::shared_ptr<Collidable>& capsuleObject, std::shared_ptr<Collidable>& polygonObject) const
{
	// オブジェクトから形状データを取得
	auto capsuleShapeData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleObject->GetCollData());
	auto polygonShapeData = std::dynamic_pointer_cast<PolygonColliderData>(polygonObject->GetCollData());
	auto& hitDim = polygonShapeData->GetHitDim();

	if (hitDim.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(hitDim);
		return;
	}

	float highestY = -FLT_MAX;

	for (int i = 0; i < hitDim.HitNum; ++i)
	{
		if (hitDim.Dim[i].Normal.y > 0.7f)
		{
			highestY = max(max(hitDim.Dim[i].Position[0].y, hitDim.Dim[i].Position[1].y), 
				max(hitDim.Dim[i].Position[2].y, highestY));
		}
	}

	if (highestY > -FLT_MAX)
	{
		// オブジェクトの位置情報は、Collidableである capsuleObject から取得
		Vector3 nextPos = capsuleObject->m_nextPos;
		if (nextPos.y < highestY)
		{
			nextPos.y = highestY;
			capsuleObject->m_nextPos = nextPos;

			auto& rb = capsuleObject->GetRigidbody();
			auto velo = rb.GetVelo();
			if (velo.y < 0)
			{
				velo.y = 0.0f;  
				capsuleObject->GetRigidbody().SetOnGround(true);
				rb.SetVelo(velo);
			}
		}
	}

	// 結果の解放
	//MV1CollResultPolyDimTerminate(hitDim);
}

void Physics::FixNextPositionCapsuleSlope(std::shared_ptr<Collidable>& capsuleObject, std::shared_ptr<Collidable>& polygonObject) const
{
	// オブジェクトから形状データを取得
	auto capsuleShapeData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleObject->GetCollData());
	auto polygonShapeData = std::dynamic_pointer_cast<PolygonColliderData>(polygonObject->GetCollData());
	auto& hitDim = polygonShapeData->GetHitDim();

	if (hitDim.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(hitDim);
		return;
	}

	Vector3 totalPushBack(0, 0, 0);
	Vector3 slideNormal(0, 0, 0);
	int hitCount = 0;

	for (int i = 0; i < hitDim.HitNum; ++i)
	{
		if (hitDim.Dim[i].Normal.y > 0.1f && hitDim.Dim[i].Normal.y < 0.95f)
		{
			Vector3 headPos = capsuleShapeData->GetNextStartPos(capsuleObject->GetRigidbody().GetVelo());
			Vector3 legPos = capsuleObject->m_nextPos;

			float distSq = Segment_Triangle_MinLength_Square(
				headPos.ToDxVECTOR(),
				legPos.ToDxVECTOR(),
				hitDim.Dim[i].Position[0],
				hitDim.Dim[i].Position[1],
				hitDim.Dim[i].Position[2]
			);

			float overlap = capsuleShapeData->GetRadius() - sqrtf(distSq);

			if (overlap > 0)
			{
				Vector3 normal(
					hitDim.Dim[i].Normal.x,
					hitDim.Dim[i].Normal.y,
					hitDim.Dim[i].Normal.z
				);

				totalPushBack += normal * (overlap + kOverlapGap);
				slideNormal += normal;
				hitCount++;
			}
		}
	}

	if (hitCount > 0)
	{
		// オブジェクトの位置(m_nextPos)を更新
		capsuleObject->m_nextPos += totalPushBack / static_cast<float>(hitCount);

		// 坂道に押し戻されている＝地面の上にいる、と判定する
		capsuleObject->GetRigidbody().SetOnGround(true);

		auto& rb = capsuleObject->GetRigidbody();
		Vector3 velo = rb.GetVelo();
		slideNormal.Normalize();

		float dot = velo.Dot(slideNormal);
		if (dot < 0)
		{
			Vector3 slideVelo = velo - slideNormal * dot;
			rb.SetVelo(slideVelo); 
		}
	}

	//MV1CollResultPolyDimTerminate(hitDim);
}

bool Physics::SkipCheckCollide(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const
{
	if (collA->GetTag() == ObjectTag::Player       && collB->GetTag() == ObjectTag::PlayerWeapon ||
		collA->GetTag() == ObjectTag::PlayerWeapon && collB->GetTag() == ObjectTag::Player       ||
		collA->GetTag() == ObjectTag::Player       && collB->GetTag() == ObjectTag::PlayerMagic  ||
		collA->GetTag() == ObjectTag::PlayerMagic  && collB->GetTag() == ObjectTag::Player       ||
		collA->GetTag() == ObjectTag::Enemy		   && collB->GetTag() == ObjectTag::EnemyWeapon  ||
		collA->GetTag() == ObjectTag::EnemyWeapon  && collB->GetTag() == ObjectTag::Enemy		 ||
		collA->GetTag() == ObjectTag::Boss		   && collB->GetTag() == ObjectTag::EnemyWeapon  ||
		collA->GetTag() == ObjectTag::EnemyWeapon  && collB->GetTag() == ObjectTag::Boss)
	{
		return true;
	}
	return false;
}

bool Physics::SkipFixPos(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const
{
	if (collA->GetTag() == ObjectTag::PlayerWeapon && collB->GetTag() == ObjectTag::Enemy        ||
		collA->GetTag() == ObjectTag::Enemy        && collB->GetTag() == ObjectTag::PlayerWeapon ||
		collA->GetTag() == ObjectTag::PlayerMagic  && collB->GetTag() == ObjectTag::Enemy        ||
		collA->GetTag() == ObjectTag::Enemy		   && collB->GetTag() == ObjectTag::PlayerMagic  ||
		collA->GetTag() == ObjectTag::PlayerWeapon && collB->GetTag() == ObjectTag::Boss		 ||
		collA->GetTag() == ObjectTag::Boss		   && collB->GetTag() == ObjectTag::PlayerWeapon ||
		collA->GetTag() == ObjectTag::PlayerMagic  && collB->GetTag() == ObjectTag::Boss		 ||
		collA->GetTag() == ObjectTag::Boss		   && collB->GetTag() == ObjectTag::PlayerMagic  ||
		collA->GetTag() == ObjectTag::Player	   && collB->GetTag() == ObjectTag::EnemyWeapon  ||
		collA->GetTag() == ObjectTag::EnemyWeapon  && collB->GetTag() == ObjectTag::Player)
	{
		return true;
	}
	return false;
}

bool Physics::ShouldCallOnCollide(ObjectTag tagA, ObjectTag tagB) const
{
	if (tagA == ObjectTag::Player	  && tagB == ObjectTag::Enemy	   ||
		tagA == ObjectTag::Enemy      && tagB == ObjectTag::Player	   ||
		tagA == ObjectTag::Player	  && tagB == ObjectTag::Boss	   ||
		tagA == ObjectTag::Boss		  && tagB == ObjectTag::Player	   ||
		tagA == ObjectTag::Player     && tagB == ObjectTag::Floor      ||
		tagA == ObjectTag::Floor	  && tagB == ObjectTag::Player     ||
		tagA == ObjectTag::Player	  && tagB == ObjectTag::Wall	   ||
		tagA == ObjectTag::Wall		  && tagB == ObjectTag::Player	   ||
		tagA == ObjectTag::Player	  && tagB == ObjectTag::Slope	   ||
		tagA == ObjectTag::Companion  && tagB == ObjectTag::Wall	   ||
		tagA == ObjectTag::Wall		  && tagB == ObjectTag::Companion  ||
		tagA == ObjectTag::Companion  && tagB == ObjectTag::Floor	   ||
		tagA == ObjectTag::Floor      && tagB == ObjectTag::Companion  ||
		tagA == ObjectTag::Slope      && tagB == ObjectTag::Companion  ||
		tagA == ObjectTag::Enemy	  && tagB == ObjectTag::Wall	   ||
		tagA == ObjectTag::Wall		  && tagB == ObjectTag::Enemy      ||
		tagA == ObjectTag::Enemy      && tagB == ObjectTag::Floor	   ||
		tagA == ObjectTag::Floor	  && tagB == ObjectTag::Enemy      ||
		tagA == ObjectTag::Enemy	  && tagB == ObjectTag::Slope	   ||
		tagA == ObjectTag::Slope	  && tagB == ObjectTag::Enemy      ||
		tagA == ObjectTag::Boss		  && tagB == ObjectTag::Wall	   ||
		tagA == ObjectTag::Wall		  && tagB == ObjectTag::Boss	   ||
		tagA == ObjectTag::Boss		  && tagB == ObjectTag::Floor	   ||
		tagA == ObjectTag::Floor	  && tagB == ObjectTag::Boss	   ||
		tagA == ObjectTag::Boss		  && tagB == ObjectTag::Slope	   ||
		tagA == ObjectTag::Slope      && tagB == ObjectTag::Boss	   ||
		tagA == ObjectTag::Enemy	  && tagB == ObjectTag::Enemy	   ||
		tagA == ObjectTag::Enemy	  && tagB == ObjectTag::Boss	   ||
		tagA == ObjectTag::Boss		  && tagB == ObjectTag::Enemy)
	{
		return false;
	}
	return true;
}

void Physics::SegmentClosestPoint(Vector3& segAStart, Vector3& segAEnd, Vector3& segBStart, Vector3& segBEnd, Vector3* closestPtA, Vector3* closestPtB) const
{
	// 線分Aの方向ベクトル
	Vector3 segADir = segAEnd - segAStart;
	// 線分Bの方向ベクトル
	Vector3 segBDir = segBEnd - segBStart;
	// 線分Aの始点から線分Bの始点
	Vector3 startDiff = segAStart - segBStart;

	// 線分Aの長さ2乗
	float segASqLen = segADir.Dot(segADir);
	// 線分Bの長さ2乗
	float segBSqLen = segBDir.Dot(segBDir);
	// B始点→A始点ベクトルが、線分Bにどれだけ沿っているか
	float segBDotDiff = segBDir.Dot(startDiff);

	float paramA, paramB;

	// 両方の線分が点の場合
	if (segASqLen <= 0.000001f && segBSqLen <= 0.000001f)
	{
		// 双方の始点を最近接点にする
		*closestPtA = segAStart;
		*closestPtB = segBStart;
		return;
	}

	// 線分Aが点の場合
	if (segASqLen <= 0.000001f)
	{
		paramA = 0.0f;
		// 線分B上の最近接点の計算をし、0~1に制限
		paramB = segBDotDiff / segBSqLen;
		paramB = std::clamp(paramB, 0.0f, 1.0f);
	}
	else
	{
		// Aに向かうベクトルとA始点、B始点の内積
		float segADotDiff = segADir.Dot(startDiff);

		// 線分Bが点の場合
		if (segBSqLen <= 0.000001f)
		{
			paramB = 0.0f;
			// 線分A上の最近接点の計算をし、0~1に制限
			paramA = -segADotDiff / segASqLen;
			paramA = std::clamp(paramA, 0.0f, 1.0f);
		}
		else
		{
			// Aに向かうベクトルとBに向かうベクトルの内積
			float segABDot = segADir.Dot(segBDir);
			// 二つの線分がどれだけ平行じゃないか(0に近いほど平行)
			float segmentParallel = segASqLen * segBSqLen - segABDot * segABDot;

			// 平行じゃない場合
			if (segmentParallel != 0.0f)
			{
				paramA = (segABDot * segBDotDiff - segADotDiff * segBSqLen) / segmentParallel;
				paramA = std::clamp(paramA, 0.0f, 1.0f);
			}
			else
			{
				// 平行の場合はA始点を仮の最近接点とする
				paramA = 0.0f;
			}

			// 線分B上の最近接点の計算
			paramB = (segABDot * paramA + segBDotDiff) / segBSqLen;

			// 線分Bの外にいる場合、補正してA側を調整
			if (paramB < 0.0f)
			{
				paramB = 0.0f;
				paramA = std::clamp(-segADotDiff / segASqLen, 0.0f, 1.0f);
			}
			else if (paramB > 1.0f)
			{
				paramB = 1.0f;
				paramA = std::clamp((segABDot - segADotDiff) / segASqLen, 0.0f, 1.0f);
			}
		}
	}

	// 線分上の最近接点座標を代入
	*closestPtA = segAStart + (segADir * paramA);
	*closestPtB = segBStart + (segBDir * paramB);
}
