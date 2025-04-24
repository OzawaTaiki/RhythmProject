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
}

void NotesSystem::Update(float _deltaTime)
{
    for (auto it = notes_.begin(); it != notes_.end();)
    {
        if (((*it)->GetPosition().z - noteSize_) < judgeLinePosition_ - (noteSpeed_ * missJudgeThreshold_))
        {
            // 判定を行う
            JudgeResultData noteJudgeData(NoteJudgeType::Miss,(*it)->GetLaneIndex());
            EventManager::GetInstance()->DispatchEvent(
                GameEvent("JudgeResult", &noteJudgeData)
            );
            (*it)->Judge();
        }

        // 判定済みのノーツは削除
        if ((*it)->IsJudged())
        {
            it = notes_.erase(it);
            continue;
        }

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

void NotesSystem::CreateNormalNote(uint32_t _laneIndex, float _speed,float _targetTime)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.z = judgeLinePosition_ + _targetTime * _speed;

    laneStartPosition.y += noteSize_ / 2.0f;
    auto note = std::make_shared<NomalNote>();
    note->Initilize(laneStartPosition, _speed, _targetTime,_laneIndex);

    notes_.emplace_back(note);

    lane_->AddNote(_laneIndex, note);
}

void NotesSystem::CreateLongNote(uint32_t _laneIndex, float _speed, float _targetTime, std::shared_ptr<Note> _nextNote)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.z = judgeLinePosition_ + _targetTime * _speed;
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

void NotesSystem::DebugWindow()
{
#ifdef _DEBUG

    ImGui::PushID(this);

    static int laneIndex = 0;
    ImGui::InputInt("LaneIndex", &laneIndex);
    ImGui::DragFloat("NoteSpeed", &noteSpeed_, 0.01f);
    static float targetTime = 0.0f;
    ImGui::DragFloat("TargetTime", &targetTime, 0.01f);

    if (ImGui::Button("CreateNormalNote"))
    {
        CreateNormalNote(laneIndex, noteSpeed_, targetTime);
        stopwatch_->Reset();
    }

    if (ImGui::Button("CreateLongNote"))
    {
        Vector3 laneStartPosition = lane_->GetLaneStartPosition(laneIndex);
        laneStartPosition.z = judgeLinePosition_ + (targetTime + 1.0f) * noteSpeed_;
        laneStartPosition.y += noteSize_ / 2.0f;
        /// デバッグ用
        auto nextNote = std::make_shared<Note>();
        nextNote->Initilize(laneStartPosition, noteSpeed_, targetTime + 1.0f , laneIndex);

        CreateLongNote(laneIndex, noteSpeed_, targetTime, nextNote);
        notes_.emplace_back(nextNote);
        lane_->AddNote(laneIndex, nextNote);
        stopwatch_->Reset();
    }

    ImGui::PopID();

#endif // _DEBUG
}
