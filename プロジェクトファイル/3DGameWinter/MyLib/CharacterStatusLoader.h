#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include "Actor/Character/Status.h"

class CharacterStatusLoader
{
public:
	// 指定名のステータスを返す。見つからない場合はデフォルト値を返す。
	static Status GetStatus(const std::string& name);

	// 明示的にファイルから読み込む（テストや初期化時に呼べる）
	static void LoadFromFile(const std::string& path);

private:
	static void EnsureLoaded();

	static std::unordered_map<std::string, Status> s_statusMap;
	static std::once_flag s_loadFlag;
	static std::string s_defaultPath;
};