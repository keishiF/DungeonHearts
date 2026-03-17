#include "Options.h"

#include <fstream>
#include <sstream>
#include <string>

Options::Options()
{
}

Options& Options::GetInstance()
{
	static Options opts;
	return opts;
}

void Options::Load()
{
	std::ifstream ifs("options.cfg");
	if (!ifs) return;

	std::string line;
	while (std::getline(ifs, line))
	{
		std::istringstream iss(line);
		std::string key;
		if (!std::getline(iss, key, '=')) continue;

		std::string value;
		if (!std::getline(iss, value)) continue;

		if (key == "invertX")		invertX = (value == "1");
		else if (key == "invertY")		invertY = (value == "1");
		else if (key == "masterVolume") masterVolume = std::stoi(value);
		else if (key == "bgmVolume")	bgmVolume = std::stoi(value);
		else if (key == "seVolume")		seVolume = std::stoi(value);
	}
}

void Options::Save()
{
	std::ofstream ofs("options.cfg");
	ofs << "invertX=" << (invertX ? "1" : "0") << "\n";
	ofs << "invertY=" << (invertY ? "1" : "0") << "\n";
	ofs << "masterVolume=" << masterVolume << "\n";
	ofs << "bgmVolume=" << bgmVolume << "\n";
	ofs << "seVolume=" << seVolume << "\n";
}
