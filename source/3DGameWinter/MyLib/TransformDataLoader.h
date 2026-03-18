#pragma once
#include <DxLib.h>
#include <string>
#include <vector>

struct ObjectData
{
    std::string name;                         // オブジェクト名
    VECTOR pos = VGet(0.0f, 0.0f, 0.0f);	  // 位置
    VECTOR rot = VGet(0.0f, 0.0f, 0.0f);      // 回転
    VECTOR scale = VGet(0.0f, 0.0f, 0.0f);	  // 拡大量
    int    groupId = 0;                       // どのグループに属しているか
};

class TransformDataLoader
{
public:
    TransformDataLoader();
    virtual ~TransformDataLoader();

    /// <summary>
    /// CSVファイルからオブジェクトのTranceform情報を取得
    /// </summary>
    /// <param name="fileName">csv</param>
    /// <returns></returns>
    static std::vector<ObjectData> LoadDataCSV(const char* fileName);
};

