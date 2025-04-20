#include "NoteKeyController.h"

#include <Features/Event/EventManager.h>

#include <Application/EventData/HitKeyData.h>

void NoteKeyController::Initialize(Stopwatch* _stopWatch, NoteJudge* _noteJudge)
{
    input_ = Input::GetInstance();

    if (_stopWatch == nullptr)        throw std::runtime_error("Stopwatch is not initialized.");
    stopWatch_ = _stopWatch;

    if (_noteJudge == nullptr)        throw std::runtime_error("NoteJudge is not initialized.");
    noteJudge_ = _noteJudge;

    InitializeJsonBinder();

}

void NoteKeyController::Update()
{
    uint32_t laneIndex = 0xffffffff;

    if (input_->IsKeyTriggered(laneKeyBindings_[0]))
    {
        laneIndex = 0;
    }
    if (input_->IsKeyTriggered(laneKeyBindings_[1]))
    {
        laneIndex = 1;
    }
    if (input_->IsKeyTriggered(laneKeyBindings_[2]))
    {
        laneIndex = 2;
    }
    if (input_->IsKeyTriggered(laneKeyBindings_[3]))
    {
        laneIndex = 3;
    }

    if (laneIndex != 0xffffffff)
    {
        EventManager::GetInstance()->DispatchEvent(
            GameEvent("HitKey", new HitKeyData(laneIndex, stopWatch_->GetElapsedTime<double>()))
        );
    }

}

void NoteKeyController::Draw()
{
}

void NoteKeyController::InitializeJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("noteKeyController", "Resources/Data/KeyController/");

    jsonBinder_->RegisterVariable("laneKeyBindings", &laneKeyBindings_);

    if (laneKeyBindings_.size() == 0)
    {
        laneKeyBindings_.resize(4);
        laneKeyBindings_[0] = DIK_D;    // 左から1番目
        laneKeyBindings_[1] = DIK_F;    // 左から2番目
        laneKeyBindings_[2] = DIK_J;    // 左から3番目
        laneKeyBindings_[3] = DIK_K;    // 左から4番目
    }

}
