#pragma once
#include "StageObjectBase.h"

class WallFront : public StageObjectBase
{
public:
    WallFront(int model);
    virtual ~WallFront();

    void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
    void Update() override;
    void Draw() override;
    void OnDamage(int atk) override;

private:
};

