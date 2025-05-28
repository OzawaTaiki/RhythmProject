#include "BeatMapLoader.h"

#include <json.hpp>
#include <fstream>

BeatMapLoader::BeatMapLoader()
{
}

BeatMapLoader::~BeatMapLoader()
{
}

BeatMapLoader* BeatMapLoader::GetInstance()
{
    static BeatMapLoader instance;
    return &instance;
}

std::future<bool> BeatMapLoader::LoadBeatMap(const std::string& filePath)
{
    // すでにロード中なら拒否
    if (isLoading_) {
        std::promise<bool> promise;
        promise.set_value(false);
        return promise.get_future();
    }

    // ロード状態初期化
    isLoading_ = true;
    isLoadingSuccess_ = false;
    errorMessage_.clear();

    // 非同期処理を開始
    return std::async(std::launch::async, [this, filePath]() {
        try {
            // ファイル読み込み
            std::ifstream file(filePath);
            if (!file.is_open()) {
                errorMessage_ = "Could not open file: " + filePath;
                isLoading_ = false;
                return false;
            }

            // JSONパース
            nlohmann::json jsonData;
            file >> jsonData;
            file.close();

            // 譜面データ変換
            loadedBeatMapdata_ = ParseJsonToBeatMap(jsonData);

            // 成功フラグ設定
            isLoadingSuccess_ = true;

        }
        catch (const std::exception& e) {
            // エラー発生時
            errorMessage_ = e.what();
            isLoadingSuccess_ = false;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2)); // 2s待機
        // ロード完了
        isLoading_ = false;

        return isLoadingSuccess_;
        });
}


BeatMapData BeatMapLoader::ParseJsonToBeatMap(const nlohmann::json& _jsonData)
{
    BeatMapData data;

    // 曲のタイトル
    if (_jsonData.contains("title") && _jsonData["title"].is_string())
    {
        data.title = _jsonData["title"].get<std::string>();
    }
    else
    {
        errorMessage_ = ("Error: title is not found or not a string.");
        return data;
    }

    // アーティスト名
    if (_jsonData.contains("artist") && _jsonData["artist"].is_string())
    {
        data.artist = _jsonData["artist"].get<std::string>();
    }
    // 音声ファイルのパス
    if (_jsonData.contains("audioFilePath") && _jsonData["audioFilePath"].is_string())
    {
        data.audioFilePath = _jsonData["audioFilePath"].get<std::string>();
    }
    else
    {
        errorMessage_ = ("Error: audioFilePath is not found or not a string.");
        return data;
    }
    // BPM
    if (_jsonData.contains("bpm") && _jsonData["bpm"].is_number())
    {
        data.bpm = _jsonData["bpm"].get<float>();
    }
    else
    {
        errorMessage_ = ("Error: bpm is not found or not a number.");
        return data;
    }
    // オフセット時間
    if (_jsonData.contains("offset") && _jsonData["offset"].is_number())
    {
        data.offset = _jsonData["offset"].get<float>();
    }
    // 難易度レベル
    if (_jsonData.contains("difficultyLevel") && _jsonData["difficultyLevel"].is_number())
    {
        data.difficultyLevel = _jsonData["difficultyLevel"].get<uint32_t>();
    }
    // ノートデータ
    if (_jsonData.contains("notes") && _jsonData["notes"].is_array())
    {
        for (const auto& note : _jsonData["notes"])
        {
            NoteData noteData;
            if (note.contains("laneIndex") && note["laneIndex"].is_number())
            {
                noteData.laneIndex = note["laneIndex"].get<uint32_t>();
            }
            if (note.contains("targetTime") && note["targetTime"].is_number())
            {
                noteData.targetTime = note["targetTime"].get<float>();
            }
            if (note.contains("noteType") && note["noteType"].is_string())
            {
                noteData.noteType = note["noteType"].get<std::string>();
            }
            if (note.contains("holdDuration") && note["holdDuration"].is_number())
            {
                noteData.holdDuration = note["holdDuration"].get<float>();
            }
            data.notes.push_back(noteData);
        }
    }
    else
    {
        errorMessage_ = ("Error: notes is not found or not an array.");
        return data;
    }

    // dataをtargetTImeの昇順でソート
    std::sort(data.notes.begin(), data.notes.end(), [](const NoteData& a, const NoteData& b) {
        return a.targetTime < b.targetTime;
        });

    return data;
}
