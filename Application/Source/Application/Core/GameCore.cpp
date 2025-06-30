#include "GameCore.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Math/MyLib.h>
#include <Debug/Debug.h>

// TODO : 判定ラインパーフェクトでない
// 判定ラインでパーフェクトにならない 一本目の黄色ラインがそうなっている なぁぜ

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

    noteJudge_ = std::make_unique<NoteJudge>();
    noteJudge_->Initialize();

    judgeResult_ = std::make_unique<JudgeResult>();
    judgeResult_->Initialize();

    noteJudge_->SetLaneTotalWidth(Lane::GetTotalWidth());


    noteDeletePosition_ = -noteJudge_->GetMissJudgeThreshold() * noteSpeed_; // ノーツを削除する位置を設定
}

void GameCore::Update(float  _deltaTime, const std::vector<InputDate>& _inputData)
{
    for (auto& lane : lanes_)
    {
        judgeResult_->AddJudge(JudgeType::Miss, lane->DeleteNotesOutOfScreen(noteDeletePosition_));
    }

    JudgeNotes(_inputData);

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
#ifdef _DEBUG
    ImGui::DragFloat("speed", &noteSpeed_, 0.01f);
#endif // _DEBUG
    noteJudge_->SetSpeed(noteSpeed_);
    for (auto& lane : lanes_)
    {
        lane->Update(elapsedTime, noteSpeed_);
    }
}

void GameCore::Draw(const Camera* _camera)
{
    LineDrawer::GetInstance()->RegisterPoint(Vector3(-4, 0, 0), Vector3(4, 0, 0));

    noteJudge_->DrawJudgeLine();

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

void GameCore::JudgeNotes(const std::vector<InputDate>& _inputData)
{
    for (auto& inputdata : _inputData)
    {
        int32_t laneIndex = inputdata.laneIndex;
        if (laneIndex < 0 || laneIndex >= static_cast<int32_t>(lanes_.size()))
        {
            // エラー処理: レーンインデックスが範囲外
            throw std::out_of_range("InputData laneIndex is out of range.");
            return;
        }

        auto& lane = lanes_[laneIndex];

        auto note = lane->GetFirstNote();
        if (!note)
            continue;

        JudgeType result = JudgeType::None; // 初期化
        if (note->GetNoteType() == NoteType::Normal||
            note->GetNoteType() == NoteType::Hold)
        {
            if(inputdata.state == KeyState::trigger)
                result = noteJudge_->ProcessNoteJudge(note, inputdata.elapsedTime);
        }
        else if (note->GetNoteType() == NoteType::HoldEnd)
        {
            if (inputdata.state == KeyState::Released)
            {
                result = noteJudge_->ProcessNoteJudge(note, inputdata.elapsedTime);
            }
        }

        if (result == JudgeType::None)
            continue; // 判定なしの場合はスキップ

        judgeResult_->AddJudge(result);
        note->Judge();

        if(onJudgeCallback_)
            onJudgeCallback_(laneIndex); // 判定時のコールバックを呼び出す
    }
}

void GameCore::ParseBeatMapData(const BeatMapData& _beatMapData)
{
    notesPerLane_.clear(); // 既存のデータをクリア

    // レーンごとのでーたに分ける
    notesPerLane_.resize(lanes_.size());

    for (const auto& note : _beatMapData.notes)
    {
        if (note.laneIndex >= notesPerLane_.size())
        {
            // エラー処理: レーンインデックスが範囲外
            throw std::out_of_range("Note laneIndex is out of range.");
            return;
        }

        notesPerLane_[note.laneIndex].push_back(note);
    }

    // 時間でソート
    for (auto& notes : notesPerLane_)
    {
        notes.sort([](const NoteData& a, const NoteData& b) {
            return a.targetTime < b.targetTime; // 昇順でソート
            });
    }

    CreateBeatMapNotes();
}

void GameCore::CreateBeatMapNotes()
{
    for (int32_t index = 0; index < notesPerLane_.size(); ++index)
    {
        lanes_[index]->Initialize(notesPerLane_[index], index, 0.0f, noteSpeed_, offset_);// TODO 仮の値を使用している 判定ラインの座標
    }
}

void GameCore::Restart(std::shared_ptr<VoiceInstance> _voiceInstance)
{
    SetMusicVoiceInstance(_voiceInstance);
    CreateBeatMapNotes();
    judgeResult_->Initialize();
}
