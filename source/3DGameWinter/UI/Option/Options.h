#pragma once

class Options
{
public:
	static Options& GetInstance();

	bool invertX = false;
	bool invertY = false;

	// 内部的なボリュームは25単位で保持（0..250）
	int masterVolume = 125; //0〜250 を25単位で保持
	int bgmVolume = 125;
	int seVolume = 125;

	void Load();
	void Save();

	static constexpr int VolumeStep = 25;
	static constexpr int VolumeMax = VolumeStep * 10; //250

private:
	Options();
};
