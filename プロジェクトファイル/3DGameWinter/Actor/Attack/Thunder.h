#pragma once
#include "Magic.h"

class Thunder : public Magic
{
public:
	Thunder();
	virtual ~Thunder();

	virtual void Init(const Vector3& pos, const Vector3& targetPos) override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:
	int m_thunderEffect;
	int m_playingEffect;
};

