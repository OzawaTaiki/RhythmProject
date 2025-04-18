#include "NotesSystem.h"

#include <Debug/ImGuiDebugManager.h>

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
    for (auto& note : notes_)
    {
        note->Update(_deltaTime);
    }
}

void NotesSystem::DrawNotes(const Camera* _camera)
{
    for (auto& note : notes_)
    {
        note->Draw(_camera);
    }
}

void NotesSystem::CreateNote(uint32_t _laneIndex, float _speed)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(_laneIndex);
    laneStartPosition.y += noteSize_ / 2.0f;
    auto note = std::make_shared<nomalNote>();
    note->Initilize(laneStartPosition, _speed);
    notes_.emplace_back(note);
}

void NotesSystem::DebugWindow()
{
#ifdef _DEBUG

    ImGui::PushID(this);

    static int laneIndex = 0;
    ImGui::InputInt("LaneIndex", &laneIndex);
    ImGui::DragFloat("NoteSpeed", &noteSpeed_, 0.01f);

    if (ImGui::Button("CreateNote"))
    {
        CreateNote(laneIndex, noteSpeed_);
    }

    ImGui::PopID();

#endif // _DEBUG
}
