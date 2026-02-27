#pragma once
#include "ColliderData.h"
#include "MyLib/Physics/ObjectTag.h"
#include "MyLib/Physics/Rigidbody.h"
#include <memory>

class Physics;
class Collidable abstract : public std::enable_shared_from_this<Collidable>
{
public:
	Collidable(ObjectTag tag, ColliderData::Kind colliderKind);
	virtual ~Collidable();
	virtual void Init();
	virtual void Final();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;
	virtual int GetAttackPower() const { return 0; }

	Rigidbody& GetRigidbody() { return m_rigidbody; }
	float GetColRadius() const { return m_colRadius; }
	std::shared_ptr<ColliderData>GetCollData() const { return m_colliderData; }
	ObjectTag GetTag() const { return m_tag; }
	Vector3 GetNextPos() const { return m_nextPos; }
	void SetActive(bool active) { m_isActive = active; }
	bool IsActive() const { return m_isActive; }

protected:
	Rigidbody m_rigidbody;
	float m_colRadius;

	std::shared_ptr<ColliderData> m_colliderData;

private:
	std::shared_ptr<ColliderData> CreateColliderData(ColliderData::Kind kind);

	ObjectTag m_tag;

	Vector3 m_nextPos;

	bool m_isActive;

private:
	friend Physics;
};

