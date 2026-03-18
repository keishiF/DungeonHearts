#include "StageController.h"

#include "Floor.h"
#include "MyLib/TransformDataLoader.h"
#include "StageObjectBase.h"
#include "WallFront.h"
#include "WallSide.h"

namespace
{
}

StageController::StageController() :
	m_floorModel(-1),
	m_wallFrontModel(-1),
	m_wallSideModel(-1)
{
}

StageController::~StageController()
{
	for (auto& floor : m_floors)
	{
		floor->Final();
	}
	for (auto& wallFront : m_wallFronts)
	{
		wallFront->Final();
	}
	for (auto& wallSide : m_wallSides)
	{
		wallSide->Final();
	}

	if (m_floorModel != -1) MV1DeleteModel(m_floorModel);
	if (m_wallFrontModel != -1) MV1DeleteModel(m_wallFrontModel);
	if (m_wallSideModel != -1) MV1DeleteModel(m_wallSideModel);
}

void StageController::Init(const char* stageDataPath)
{
	TransformDataLoader loader;
	auto transformDataList = loader.LoadDataCSV(stageDataPath);

	m_floorModel = MV1LoadModel("Data/Model/Stage/Floor.mv1");
	assert(m_floorModel >= 0);
	m_wallFrontModel = MV1LoadModel("Data/Model/Stage/WallFront.mv1");
	assert(m_wallFrontModel >= 0);
	m_wallSideModel = MV1LoadModel("Data/Model/Stage/WallSide.mv1");
	assert(m_wallSideModel >= 0);

	for (const auto& data : transformDataList)
	{
		std::string modelPath;

		if (data.name == "Floor")
		{
			auto floor = std::make_shared<Floor>(m_floorModel);
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y , data.scale.z };
			floor->Init(pos, rot, scale);
			m_floors.emplace_back(floor);
		}
		else if (data.name == "WallFront")
		{
			auto wall = std::make_shared<WallFront>(m_wallFrontModel);
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y, data.scale.z };
			wall->Init(pos, rot, scale);
			m_wallFronts.emplace_back(wall);
		}
		else if (data.name == "WallSide")
		{
			auto wall = std::make_shared<WallSide>(m_wallSideModel);
			Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
			Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
			Vector3 scale = { data.scale.x, data.scale.y, data.scale.z };
			wall->Init(pos, rot, scale);
			m_wallSides.emplace_back(wall);
		}
	}
}

void StageController::Update()
{
}

void StageController::Draw()
{
	for (auto& floor : m_floors)
	{
		floor->Draw();
	}

	for (auto& wallFront: m_wallFronts)
	{
		wallFront->Draw();
	}

	for (auto& wallSide : m_wallSides)
	{
		wallSide->Draw();
	}
}

std::list<std::shared_ptr<StageObjectBase>> StageController::GetAllStageObjects() const
{
	std::list<std::shared_ptr<StageObjectBase>> allObjects;
	for (const auto& floor : m_floors)
	{
		allObjects.push_back(floor);
	}
	for (const auto& wallFront : m_wallFronts)
	{
		allObjects.push_back(wallFront);
	}
	for (const auto& wallSide : m_wallSides)
	{
		allObjects.push_back(wallSide);
	}
	return allObjects;
}
