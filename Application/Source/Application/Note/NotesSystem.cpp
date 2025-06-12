#include "NotesSystem.h"

#include <Debug/ImGuiDebugManager.h>

#include <Features/Event/EventManager.h>

#include <Application/EventData/JudgeResultData.h>

NotesSystem::NotesSystem(Lane* _lane) : lane_(_lane), playing_(false)
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("NotesSystem", [&]() { DebugWindow(); });
#endif // _DEBUG
}

NotesSystem::~NotesSystem()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("NotesSystem");
#endif // _DEBUG
}

void NotesSystem::Initialize(float _noteSpeed, float _noteSize)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    noteSpeed_ = _noteSpeed;
    noteSize_ = _noteSize;

    notes_.clear();

    Model::CreateFromFile("cube/cube.obj");

}

void NotesSystem::Update(float _deltaTime)
{
    for (auto it = notes_.begin(); it != notes_.end();)
    {
        // 判定済みのノーツは削除
        if ((*it)->IsJudged())
        {
            it = notes_.erase(it);
            continue;
        }

        // ノーツは次のフレームで消す
        if (((*it)->GetPosition().z - noteSize_ / 2.0f) < judgeLinePosition_ - (noteSpeed_ * missJudgeThreshold_))
        {
            // 判定を行う
            JudgeResultData noteJudgeData(NoteJudgeType::Miss, (*it)->GetLaneIndex());
            EventManager::GetInstance()->DispatchEvent(
                GameEvent("JudgeResult", &noteJudgeData)
            );
            (*it)->Judge();
        }
        else if (playing_)
            (*it)->Update(_deltaTime);

        ++it;
    }
}

void NotesSystem::DrawNotes(const Camera* _camera)
{
    for (auto& note : notes_)
    {
        note->Draw(_camera);
    }
}

void NotesSystem::SetBeatMapDataAndCreateNotes(const BeatMapData& _beatMapData)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    beatMapData_ = _beatMapData;
    for (const auto& note : beatMapData_.notes)
    {
        if (note.noteType == "normal")
        {
            CreateNormalNote(note.laneIndex, noteSpeed_, note.targetTime);
        }
        else if (note.noteType == "hold")
        {
            CreateLongNote(note);

        }
    }
}


void NotesSystem::Reload()
{
    notes_.clear();
    lane_->Reset();
    stopwatch_->Reset();
    for (const auto& note : beatMapData_.notes)
    {
        if (note.noteType == "normal")
        {
            CreateNormalNote(note.laneIndex, noteSpeed_, note.targetTime);
        }
        else if (note.noteType == "hold")
        {
            CreateLongNote(note);
        }
    }
    stopwatch_->Start();

}

void NotesSystem::CreateNormalNote(uint32_t _laneIndex, float _speed,float _targetTime)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    float elapsedTime = stopwatch_->GetElapsedTime<float>();

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.z = judgeLinePosition_ + (_targetTime - elapsedTime) * _speed;

    laneStartPosition.y += noteSize_ / 2.0f;
    auto note = std::make_shared<NomalNote>();
    note->Initilize(laneStartPosition, _speed, _targetTime,_laneIndex);

    notes_.emplace_back(note);

    lane_->AddNote(_laneIndex, note);
}

void NotesSystem::CreateLongNote(uint32_t _laneIndex, float _speed, float _targetTime, std::shared_ptr<Note> _nextNote)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    float elapsedTime = stopwatch_->GetElapsedTime<float>();

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.z = judgeLinePosition_ + (_targetTime - elapsedTime) * _speed;
    laneStartPosition.y += noteSize_ / 2.0f;
    auto note = std::make_shared<LongNote>();

    note->Initilize(laneStartPosition, _speed, _targetTime, _laneIndex);
    notes_.emplace_back(note);
    lane_->AddNote(_laneIndex, note);


    // 次のノーツを設定
    if (_nextNote)
    {

    }
    else
    {
        /// デバッグ用
        auto nextNote = std::make_shared<LongNote>();
        float targetTime = _targetTime + 1.0f;
        laneStartPosition.z = judgeLinePosition_ + targetTime * _speed;
        nextNote->Initilize(laneStartPosition, _speed, _targetTime, _laneIndex);

        //note->SetNextNote(nextNote);

        notes_.emplace_back(nextNote);
        lane_->AddNote(_laneIndex, nextNote);
    }
}

void NotesSystem::CreateLongNote(const NoteData& _noteData)
{
    if (_noteData.holdDuration <= 0.0f)
    {
        throw std::runtime_error("Hold duration must be greater than 0 for long notes.");
        return;
    }

    // ノーツの初期化
    std::shared_ptr<LongNote> note = std::make_shared<LongNote>();

    // 経過時間
    float elapsedTime = stopwatch_->GetElapsedTime<float>();

    // ノーツの開始位置を計算
    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_noteData.laneIndex);
    // targetTimeから座標
    laneStartPosition.z = judgeLinePosition_ + (_noteData.targetTime - elapsedTime) * noteSpeed_;
    laneStartPosition.y += noteSize_ / 2.0f;

    // ノーツの初期化
    note->Initilize(laneStartPosition, noteSpeed_, _noteData.targetTime, _noteData.laneIndex);




    std::shared_ptr<LongNote> nextNote = std::make_shared<LongNote>();

    float targetTime = _noteData.targetTime + _noteData.holdDuration;

    laneStartPosition = lane_->GetLaneStartPosition(_noteData.laneIndex);
    laneStartPosition.z = judgeLinePosition_ + (targetTime - elapsedTime) * noteSpeed_;
    laneStartPosition.y += noteSize_ / 2.0f;

    nextNote->Initilize(laneStartPosition, noteSpeed_, targetTime, _noteData.laneIndex);
    nextNote->SetBeforeNote(note);

    // listとレーンに追加
    notes_.emplace_back(note);
    lane_->AddNote(_noteData.laneIndex, note);

    notes_.emplace_back(nextNote);
    lane_->AddNote(_noteData.laneIndex, nextNote);



}

std::shared_ptr<Note> NotesSystem::CreateNextNoteForLongNote(uint32_t _laneIndex, float _speed, float _targetTime)
{
    auto nextNote = std::make_shared<LongNote>();

    float elapsedTime = stopwatch_->GetElapsedTime<float>();

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.y += noteSize_ / 2.0f;

    float targetTime = _targetTime;
    laneStartPosition.z = judgeLinePosition_ + targetTime * _speed;
    nextNote->Initilize(laneStartPosition, _speed, _targetTime, _laneIndex);

    return nextNote;
}

void NotesSystem::DebugWindow()
{
#ifdef _DEBUG

    ImGui::PushID(this);

    ImGui::Checkbox("Playing", &playing_);
    if (ImGui::Button("Reload"))
    {
        Reload();
    }

    static int laneIndex = 0;
    ImGui::InputInt("LaneIndex", &laneIndex);
    ImGui::DragFloat("NoteSpeed", &noteSpeed_, 0.01f);
    static float targetTime = 0.0f;
    ImGui::DragFloat("TargetTime", &targetTime, 0.01f);

    if (ImGui::Button("CreateNormalNote"))
    {
        float elapseTime = stopwatch_->GetElapsedTime<float>();
        CreateNormalNote(laneIndex, noteSpeed_, targetTime + elapseTime);
    }

    if (ImGui::Button("CreateLongNote"))
    {
        NoteData data;
        data.holdDuration = 1.0f; // デフォルトのホールド時間
        data.laneIndex = laneIndex;
        data.targetTime = targetTime + 1 + stopwatch_->GetElapsedTime<float>();
        CreateLongNote(data);

    }

    ImGui::PopID();

#endif // _DEBUG
}
