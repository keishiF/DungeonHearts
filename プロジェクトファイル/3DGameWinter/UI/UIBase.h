#pragma once
#include "MyLib/Vector3.h"
#include <DxLib.h>
#include <memory>

/// <summary>
/// UIの基底クラス
/// </summary>
class UIBase
{
public:
    virtual ~UIBase() = default;

    virtual void Init() abstract;
    virtual void Update() abstract;
    virtual void Draw() abstract;
};

