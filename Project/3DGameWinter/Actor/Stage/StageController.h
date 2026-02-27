#pragma once
#include "MyLib/Vector3.h"
#include <memory>
#include <list>

class Floor;
class WallFront;
class WallSide;
class SkyDome;
class StageObjectBase;
class StageController
{
public:
    StageController();
    virtual ~StageController();

    void Init(const char* stageDataPath);
    void Update();
    void Draw();

    std::list<std::shared_ptr<StageObjectBase>> GetAllStageObjects() const;

private:
    std::list<std::shared_ptr<Floor>> m_floors;
    std::list<std::shared_ptr<WallFront>> m_wallFronts;
    std::list<std::shared_ptr<WallSide>> m_wallSides;

    int m_floorModel;
    int m_wallFrontModel;
    int m_wallSideModel;
};

