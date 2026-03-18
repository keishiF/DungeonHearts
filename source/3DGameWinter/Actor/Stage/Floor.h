#pragma once
#include "StageObjectBase.h"

class Floor : public StageObjectBase
{
public:
    Floor(int model);
    virtual ~Floor();

    void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
    void Update() override;
    void Draw() override;
    void OnDamage(int atk) override;

private:
};

