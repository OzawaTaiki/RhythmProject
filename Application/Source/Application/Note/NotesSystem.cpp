#include "NotesSystem.h"

#include <Debug/ImGuiDebugManager.h>

#include <Features/Event/EventManager.h>

#include <Application/EventData/JudgeResultData.h>

NotesSystem::NotesSystem(Lane* _lane) : lane_(_lane)
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
            // 次のノーツを設定
            std::shared_ptr<LongNote> nextNote = nullptr;
            if (note.holdDuration > 0.0f)
            {
                nextNote = std::make_shared<LongNote>();

                float elapsedTime = stopwatch_->GetElapsedTime<float>();

                float targetTime = note.targetTime + note.holdDuration;

                Vector3 laneStartPosition = lane_->GetLaneStartPosition(note.laneIndex);
                laneStartPosition.y += noteSize_ / 2.0f;
                laneStartPosition.z = judgeLinePosition_ + targetTime * noteSpeed_;

                nextNote->Initilize(laneStartPosition, noteSpeed_, targetTime, note.laneIndex);

                CreateLongNote(note.laneIndex, noteSpeed_, note.targetTime, nextNote);

                notes_.emplace_back(nextNote);
                lane_->AddNote(note.laneIndex, nextNote);
            }
            else
            {
                //CreateLongNote(note.laneIndex, noteSpeed_, note.targetTime, nullptr);
            }
        }
    }
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
        note->SetNextNote(_nextNote);
    }
    else
    {
        /// デバッグ用
        auto nextNote = std::make_shared<LongNote>();
        float targetTime = _targetTime + 1.0f;
        laneStartPosition.z = judgeLinePosition_ + targetTime * _speed;
        nextNote->Initilize(laneStartPosition, _speed, _targetTime, _laneIndex);

        note->SetNextNote(nextNote);

        notes_.emplace_back(nextNote);
        lane_->AddNote(_laneIndex, nextNote);
    }
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
        float elapseTime = stopwatch_->GetElapsedTime<float>();
        Vector3 laneStartPosition = lane_->GetLaneStartPosition(laneIndex);
        laneStartPosition.z = judgeLinePosition_ + (targetTime + 1.0f) * noteSpeed_;
        laneStartPosition.y += noteSize_ / 2.0f;
        /// デバッグ用
        auto nextNote = std::make_shared<Note>();
        nextNote->Initilize(laneStartPosition, noteSpeed_, targetTime + 1.0f + elapseTime, laneIndex);

        CreateLongNote(laneIndex, noteSpeed_, targetTime + elapseTime, nextNote);
        notes_.emplace_back(nextNote);
        lane_->AddNote(laneIndex, nextNote);
        //stopwatch_->Reset();
    }

    ImGui::PopID();

#endif // _DEBUG
}
