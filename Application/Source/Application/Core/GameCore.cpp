#include "GameCore.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Math/MyLib.h>
#include <Debug/Debug.h>

GameCore::GameCore(int32_t _laneCount) :
    laneCount_(_laneCount),
    isWaitingForStart_(true),
    waitTimer_(0.0f),
    noteDeletePosition_(0.0f),
    maxCombo_(0),
    combo_(0)
{
    lanes_.resize(_laneCount);
}

GameCore::~GameCore()
{
}

void GameCore::Initialize(float _noteSpeed, float _musicLaytencyMs, float _beginOffset)
{
    noteSpeed_ = _noteSpeed;
    beginOffset_ = _beginOffset;
    musicLatencyMs_ = _musicLaytencyMs;
    waitTimer_ = 0.0f;
    isWaitingForStart_ = true;


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
    noteJudge_->SetSpeed(_noteSpeed);

    noteDeletePosition_ = -noteJudge_->GetMissJudgeThreshold() * noteSpeed_; // ノーツを削除する位置を設定

    combo_ = 0; // コンボの初期化
    maxCombo_ = 0; // 最大コンボの初期化
}

void GameCore::Update(float  _deltaTime, const std::vector<InputDate>& _inputData)
{
    for (auto& lane : lanes_)
    {
        int32_t deleteCount = lane->DeleteNotesOutOfScreen(noteDeletePosition_);
        judgeResult_->AddJudge(JudgeType::Miss, deleteCount); // 削除されたノーツの数をミスとしてカウント
        if (deleteCount > 0)
        {
            combo_ = 0; // ノーツが削除されたらコンボをリセット
            if (onMissCallback_)
            {
                onMissCallback_(); // ミス時のコールバックを呼び出す
            }
        }
    }

    JudgeNotes(_inputData);

    float elapsedTime = 0.0f;
    if (isWaitingForStart_)
    {
        waitTimer_ += _deltaTime;
        elapsedTime = Lerp(-beginOffset_, 0.0f, waitTimer_ / beginOffset_);
    }
    else if (gamemusic_)
    {
        // 音楽の音声インスタンスが有効な場合、経過時間を取得
        if (gamemusic_->IsPlaying())
        {
            // 音楽が再生中の場合、経過時間を取得
            elapsedTime = gamemusic_->GetElapsedTime();
        }
    }
    for (auto& lane : lanes_)
    {
        lane->Update(elapsedTime + musicLatencyMs_ / 1000.0f, noteSpeed_);
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
    LineDrawer::GetInstance()->RegisterPoint(Vector3(-4, 0, 0), Vector3(4, 0, 0));

    noteJudge_->DrawJudgeLine();

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
        NoteType noteType= note->GetNoteType();
        switch (noteType)
        {
        case NoteType::Normal:
            result = ProcessNormalNote(note, inputdata);
            break;
        case NoteType::Hold:
            result = ProcessHoldNote(note, inputdata);
            break;
        case NoteType::HoldEnd:
            result = ProcessHoldEndNote(note, inputdata);
            break;
        default:
            break;
        }

        if (result == JudgeType::None)
            continue; // 判定なしの場合はスキップ

        RecordJudgeResult(result, note); // 判定結果を記録

        if (onJudgeCallback_)
            onJudgeCallback_(laneIndex, result); // 判定時のコールバックを呼び出す

        UpdateCombo(result);
    }
}

JudgeType GameCore::ProcessNormalNote(Note* _note, const InputDate& _inputData)
{
    // 押した瞬間だけ判定
    if (_inputData.state == KeyState::trigger)
    {
        return noteJudge_->ProcessNoteJudge(_note, _inputData.elapsedTime + musicLatencyMs_ / 1000.0f);
    }

    return JudgeType::None;
}

JudgeType GameCore::ProcessHoldNote(Note* _note, const InputDate& _inputData)
{
    // 押した瞬間だけ判定
    if (_inputData.state == KeyState::trigger)
    {
        JudgeType result = noteJudge_->ProcessNoteJudge(_note, _inputData.elapsedTime + musicLatencyMs_ / 1000.0f);

        if (result != JudgeType::None && result != JudgeType::Miss)
        {
            holdState_.StartHold(_inputData.laneIndex); // ホールド状態を開始
            Debug::Log("Hold Enable\n");
        }

        return result;
    }

    return JudgeType::None;
}

JudgeType GameCore::ProcessHoldEndNote(Note* _note, const InputDate& _inputData)
{
    if (_inputData.state == KeyState::trigger)
    {
        holdState_.StartHold(_inputData.laneIndex); // ホールド状態を開始
    }
    else if(holdState_.IsHoldingLane(_inputData.laneIndex))
    {
        Debug::Log("Hold State is holding lane: " + std::to_string(_inputData.laneIndex) + "\n");

        if(onHoldCallback_)
            onHoldCallback_(_inputData.laneIndex); // ホールド時のコールバックを呼び出す

        if (_inputData.state == KeyState::Released)
        {
            JudgeType result = noteJudge_->ProcessNoteJudge(_note, _inputData.elapsedTime + musicLatencyMs_ / 1000.0f);

            holdState_.EndHold(); // ホールド状態を終了
            // noneてことはブリッジ内なので
            if (result == JudgeType::None)
            {
                // コンボを途切れさせる
                // 判定は反映させない
                UpdateCombo(result);
            }

            return result;
        }
    }

    return JudgeType::None;
}

void GameCore::UpdateCombo(JudgeType _result)
{
    if (_result == JudgeType::Perfect ||
        _result == JudgeType::Good)
    {
        ++combo_; // コンボを増やす
        maxCombo_ = (std::max)(maxCombo_, combo_); // 最大コンボを更新
    }
    else
    {
        combo_ = 0; // コンボが途切れたらリセット
        if (onMissCallback_)
        {
            onMissCallback_(); // ミス時のコールバックを呼び出す
        }
    }
}

void GameCore::RecordJudgeResult(JudgeType _result, Note* _note)
{
    if (_result == JudgeType::None)
        return;

    judgeResult_->AddJudge(_result);
    _note->Judge();
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
        lanes_[index]->Initialize(notesPerLane_[index], index, 0.0f, noteSpeed_, beginOffset_);// TODO 仮の値を使用している 判定ラインの座標
    }
}

void GameCore::Restart()
{
    isWaitingForStart_ = true;
    waitTimer_ = 0.0f;

    CreateBeatMapNotes();
    judgeResult_->Initialize();

}
