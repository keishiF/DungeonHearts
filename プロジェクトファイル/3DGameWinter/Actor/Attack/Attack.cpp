#include "Attack.h"

Attack::Attack(ObjectTag tag, ColliderData::Kind kind) :
	Collidable(tag, kind),
	m_timer(0),
	m_durationTime(0),
	m_isDead(false),
	m_atk(0),
	m_dir(Vector3())
{
}

Attack::~Attack()
{
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
