#pragma once

#include <Features/Scene/SceneData.h>
#include <Application/BeatMapLoader/BeatMapData.h>

struct SelectToGameData : SceneData
{
    std::string selectedBeatMapFilePath; // 選択された譜面ファイルのパス
};

struct SharedBeatMapData : SceneData
{
    BeatMapData beatMapData; // 前のシーンから渡される譜面データ
};