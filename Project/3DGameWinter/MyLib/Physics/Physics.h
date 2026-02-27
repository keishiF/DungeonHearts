#pragma once
#include "Collider/Collidable.h"
#include <list>
#include <vector>

namespace
{
	//ポリゴンの当たり判定の配列の最大数
	constexpr int kMaxHitPolygon = 2048;
}

class Physics final
{
public:
	static Physics& GetInstance();

	void Entry(std::shared_ptr<Collidable> collider);
	void Exit(std::shared_ptr<Collidable> collider);
	void Update();
	void DebugDraw();

private:
	Physics();
	~Physics() = default;
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;

	int m_hitEffect;
	int m_playingEffect;
	std::list<std::shared_ptr<Collidable>> m_collidables;

	// OnCollideの衝突通知のためのデータ
	struct OnCollideInfo
	{
		std::shared_ptr<Collidable> owner;
		std::shared_ptr<Collidable> collider;
		void OnCollide() { owner->OnCollide(collider); }
	};

	std::vector<OnCollideInfo> CheckCollide() const;
	// オブジェクトが衝突しているかを判定する関数
	bool IsCollide(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const;
	bool IsCollideSS(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;
	bool IsCollideCC(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;
	bool IsCollideCS(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;
	bool IsCollideCP(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;

	// 位置補正
	void FixPosition();
	// 補正後の位置
	void FixNextPosition(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const;
	void FixNextPositionSS(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;
	void FixNextPositionCC(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;
	void FixNextPositionCP(std::shared_ptr<Collidable>& collA, std::shared_ptr<Collidable>& collB) const;

	void FixNextPositionCapsuleWall(std::shared_ptr<Collidable>& capsuleObject, 
		std::shared_ptr<Collidable>& polygonObject) const;
	void FixNextPositionCapsuleFloor(std::shared_ptr<Collidable>&capsuleObject, 
		std::shared_ptr<Collidable>& polygonObject) const;
	void FixNextPositionCapsuleSlope(std::shared_ptr<Collidable>&capsuleObject, 
		std::shared_ptr<Collidable>& polygonObject) const;

	// 当たり判定をそもそも取らないようにする
	bool SkipCheckCollide(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const;
	// 当たり判定自体はとるが押し戻しはしないようにする
	bool SkipFixPos(std::shared_ptr<Collidable> collA, std::shared_ptr<Collidable> collB) const;
	// 位置補正はするがそれ以外の処理(ダメージを受けるなど)はしないようにする
	bool ShouldCallOnCollide(ObjectTag tagA, ObjectTag tagB) const;

	// 一番近い線分を取る
	void SegmentClosestPoint(Vector3& segAStart, Vector3& segAEnd,
		Vector3& segBStart, Vector3& segBEnd,
		Vector3* closestPtA, Vector3* closestPtB) const;
};

