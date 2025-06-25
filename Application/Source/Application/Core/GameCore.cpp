#include "GameCore.h"

GameCore::GameCore(int32_t _laneCount)
{
    lanes_.resize(_laneCount);
}
GameCore::~GameCore()
{
}

void GameCore::Initialize(const BeatMapData& _beatMapData)
{

}

void GameCore::Update(float _deltaTime)
{
    float elapsedTime = 0.0f;
    if (auto voiceInstance = musicVoiceInstance_.lock())
    {
        // 音楽の音声インスタンスが有効な場合、経過時間を取得
        if (voiceInstance->IsPlaying())
        {
            // 音楽が再生中の場合、経過時間を取得
            elapsedTime = voiceInstance->GetElapsedTime();
        }
        else
        {
            // 音楽が停止している場合、経過時間は0に設定
            elapsedTime = 0.0f;
        }
    }

    for (auto& lane : lanes_)
    {
        lane->Update(elapsedTime);
    }
}

void GameCore::Draw(const Camera* _camera)
{
    for (const auto& lane : lanes_)
    {
        if (lane)
        {
            lane->Draw(_camera);
        }
    }
}

void GameCore::ParseBeatMapData(const BeatMapData& _beatMapData)
{
    // レーンごとのでーたに分ける
    std::vector<std::list<NoteData>> notesPerLane;
    notesPerLane.resize(lanes_.size());

    for (const auto& note : _beatMapData.notes)
    {
        if (note.laneIndex >= notesPerLane.size())
        {
            // エラー処理: レーンインデックスが範囲外
            throw std::out_of_range("Note laneIndex is out of range.");
            return;
        }

        notesPerLane[note.laneIndex].push_back(note);
    }

    for (int32_t index = 0; index < notesPerLane.size(); ++index)
    {
        // 時間でソート
        notesPerLane[index].sort([](const NoteData& a, const NoteData& b) {
            return a.targetTime < b.targetTime; // 昇順でソート
            });

        auto lane = std::make_unique<Lane>();
        lane->Initialize(notesPerLane[index], 0, 0.1f); // 仮の値を使用 TODO
        lanes_[index] = std::move(lane);
    }

}
