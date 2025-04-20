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

void NotesSystem::CreateNote(uint32_t _laneIndex, float _speed,float _targetTime)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.z = judgeLinePosition_ + _targetTime * _speed;

    laneStartPosition.y += noteSize_ / 2.0f;
    auto note = std::make_shared<nomalNote>();
    note->Initilize(laneStartPosition, _speed, _targetTime,_laneIndex);

    notes_.emplace_back(note);

    lane_->AddNote(_laneIndex, note);
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

    if (ImGui::Button("CreateNote"))
    {
        CreateNote(laneIndex, noteSpeed_, targetTime);
        stopwatch_->Reset();
    }

    ImGui::PopID();

#endif // _DEBUG
}
