#include "GameCore.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Math/MyLib.h>
#include <Debug/Debug.h>

GameCore::GameCore(int32_t _laneCount)
{
    lanes_.resize(_laneCount);
}

GameCore::~GameCore()
{
}

void GameCore::Initialize(float _noteSpeed, float _offset)
{
    noteSpeed_ = _noteSpeed;
    offset_ = _offset;

    // レーンの初期化
    lanes_.resize(laneCount_);
    for (int32_t i = 0; i < laneCount_; ++i)
    {
        lanes_[i] = std::make_unique<Lane>();
    }
}

void GameCore::Update(float _deltaTime)
{
    float elapsedTime = 0.0f;
    if (isWaitingForStart_)
    {
        waitTimer_ += _deltaTime;
        elapsedTime = Lerp(-offset_, 0.0f, waitTimer_ / offset_);
    }
    else if (auto voiceInstance = musicVoiceInstance_.lock())
    {
        // 音楽の音声インスタンスが有効な場合、経過時間を取得
        if (voiceInstance->IsPlaying())
        {
            // 音楽が再生中の場合、経過時間を取得
            elapsedTime = voiceInstance->GetElapsedTime();
        }
    }

    ImGui::DragFloat("speed", &noteSpeed_, 0.01f);

    for (auto& lane : lanes_)
    {
        lane->Update(elapsedTime, noteSpeed_);
    }
}

void GameCore::Draw(const Camera* _camera)
{
    LineDrawer::GetInstance()->RegisterPoint(Vector3(-4, 0, 0), Vector3(4, 0, 0));
    for (const auto& lane : lanes_)
    {
        if (lane)
        {
            lane->Draw(_camera);
        }
    }
}

void GameCore::GenerateNotes(const BeatMapData& _beatMapData)
{
    // 譜面データを解析してノーツを生成
    ParseBeatMapData(_beatMapData);
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

        lanes_[index]->Initialize(notesPerLane[index], index, 0.0f, noteSpeed_, offset_);// 仮の値を使用 TODO
    }

}
