#include "MyLib/CharacterStatusLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::unordered_map<std::string, Status> CharacterStatusLoader::s_statusMap;
std::once_flag CharacterStatusLoader::s_loadFlag;
std::string CharacterStatusLoader::s_defaultPath = "Data/CSV/CharacterStatus/CharacterStatus.csv";

static inline std::string Trim(const std::string& s)
{
	const char* ws = " \t\n\r";
	auto start = s.find_first_not_of(ws);
	if (start == std::string::npos) return "";
	auto end = s.find_last_not_of(ws);
	return s.substr(start, end - start + 1);
}

void CharacterStatusLoader::EnsureLoaded()
{
	std::call_once(s_loadFlag, []() {
		LoadFromFile(s_defaultPath);
		});
}

Status CharacterStatusLoader::GetStatus(const std::string& name)
{
	EnsureLoaded();

	auto it = s_statusMap.find(name);
	if (it != s_statusMap.end()) return it->second;

	// 見つからない場合はデフォルトの Status を返す
	return Status{};
}

void CharacterStatusLoader::LoadFromFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open()) return;

	std::string line;
	bool isHeader = true;
	while (std::getline(file, line))
	{
		if (isHeader) { isHeader = false; continue; }

		if (line.empty()) continue;

		std::stringstream ss(line);
		std::string part;
		std::vector<std::string> values;
		while (std::getline(ss, part, ',')) values.emplace_back(Trim(part));

		if (values.size() < 2) continue; // 少なくとも名前とHP

		std::string name = values[0];
		Status s{};
		try
		{
			// フォーマット: Name,HP,ATK,MaxHP,WalkSpeed,RunSpeed,AirSpeed
			s.m_hp = std::stof(values[1]);
			if (values.size() >= 3) s.m_atk = std::stof(values[2]);
			if (values.size() >= 4) s.m_maxHp = std::stof(values[3]);
			// 移動速度（省略可能）
			if (values.size() >= 5) s.m_runSpeed = std::stof(values[4]);
			if (values.size() >= 6) s.m_airSpeed = std::stof(values[5]);

			if (s.m_maxHp == 0.0f) s.m_maxHp = s.m_hp;
		}
		catch (...)
		{
			// パース失敗時はスキップ
			continue;
		}

		s_statusMap[name] = s;
	}
}