#pragma once
#include "StageObjectBase.h"

class WallSide : public StageObjectBase
{
public:
    WallSide(int model);
    virtual ~WallSide();

    void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
    void Update() override;
    void Draw() override;
    void OnDamage(int atk) override;

private:
};

