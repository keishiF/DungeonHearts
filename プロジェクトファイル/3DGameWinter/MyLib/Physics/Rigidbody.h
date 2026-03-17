#pragma once
#include "MyLib/Vector3.h"

class Rigidbody
{
public:
	Rigidbody();
	virtual ~Rigidbody();

	void Init();

	Vector3 GetPos()  const { return m_pos; }
	Vector3 GetDir()  const { return m_dir; }
	Vector3 GetVelo() const { return m_velo; }
	//次の座標
	Vector3 GetNextPos() { return m_pos + m_velo; };
	void SetOnGround(bool onGround) { m_onGround = onGround; }
	bool UseGravity() const { return m_isGravity; }

	void SetPos(Vector3 pos) { m_pos = pos; }
	void SetVelo(Vector3 velo);
	bool IsOnGround() const { return m_onGround; }
	void SetGravity(bool use) { m_isGravity = use; }

private:
	Vector3 m_pos;
	Vector3 m_dir;
	Vector3 m_velo;
	bool m_isGravity;
	bool m_onGround;
};

