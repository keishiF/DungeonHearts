#include "WallSide.h"

#include "MyLib/Physics/Collider/PolygonColliderData.h"

WallSide::WallSide(int model) :
	StageObjectBase(ObjectTag::Wall, ColliderData::Kind::Polygon)
{
	m_model = MV1DuplicateModel(model);
}

WallSide::~WallSide()
{
}

void WallSide::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	m_pos = pos;
	m_scale = scale;
	m_rot = rot;

	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	MV1SetPosition(m_model, pos.ToDxVECTOR());
	//MV1SetRotationXYZ(m_model, rot.ToDxVECTOR());
	MV1SetScale(m_model, scale.ToDxVECTOR());

	auto colData = std::static_pointer_cast<PolygonColliderData>(m_colliderData);
	colData->m_modelHandle = m_model;
}

void WallSide::Update()
{
}

void WallSide::Draw()
{
	MV1DrawModel(m_model);
}

void WallSide::OnDamage(int atk)
{
}
