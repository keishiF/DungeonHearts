#include "Floor.h"

#include "MyLib/Physics/Collider/PolygonColliderData.h"

Floor::Floor(int model) :
	StageObjectBase(ObjectTag::Floor, ColliderData::Kind::Polygon)
{
	m_model = MV1DuplicateModel(model);
}

Floor::~Floor()
{
}

void Floor::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	m_pos   = pos;
	m_scale = scale;
	m_rot   = rot;

	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(m_pos);

	MV1SetPosition(m_model, m_pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_model, m_rot.ToDxVECTOR());
	MV1SetScale(m_model, m_scale.ToDxVECTOR());

	auto colData = std::static_pointer_cast<PolygonColliderData>(m_colliderData);
	colData->m_modelHandle = m_model;
}

void Floor::Update()
{
}

void Floor::Draw()
{
	MV1DrawModel(m_model);
}

void Floor::OnDamage(int atk)
{
}
