#include "WallFront.h"

#include "MyLib/Physics/Collider/PolygonColliderData.h"

WallFront::WallFront(int model) :
	StageObjectBase(ObjectTag::Wall, ColliderData::Kind::Polygon)
{
	m_model = MV1DuplicateModel(model);
}

WallFront::~WallFront()
{
}

void WallFront::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	m_pos   = pos;
	m_scale = scale;
	m_rot   = rot;

	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	MV1SetPosition(m_model, pos.ToDxVECTOR());
	//MV1SetRotationXYZ(m_model, rot.ToDxVECTOR());
	MV1SetScale(m_model, scale.ToDxVECTOR());

	auto colData = std::static_pointer_cast<PolygonColliderData>(m_colliderData);
	colData->m_modelHandle = m_model;
}

void WallFront::Update()
{
}

void WallFront::Draw()
{
	MV1DrawModel(m_model);
}

void WallFront::OnDamage(int atk)
{
}
