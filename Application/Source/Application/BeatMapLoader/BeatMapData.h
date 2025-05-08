#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct NoteData
{
    uint32_t laneIndex; // 流れるレーンインデックス
    float targetTime; // タイミング
    std::string noteType; // ノートの種類 (normal, long)
    float holdDuration; // ホールドノートの持続時間

    NoteData() : laneIndex(0), targetTime(0.0f), noteType("normal"), holdDuration(0.0f) {}
};

// 譜面データ構造体
struct BeatMapData
{
    std::string title; // 曲のタイトル
    std::string artist; // アーティスト名 仮
    std::string audioFilePath; // 音声ファイルのパス
    float bpm; // tempo
    float offset; // オフセット時間

    uint32_t difficultyLevel; // 難易度レベル : 仮

    std::vector<NoteData> notes; // ノートデータのリスト

    BeatMapData() : bpm(0.0f), offset(0.0f), difficultyLevel(0) {}

};