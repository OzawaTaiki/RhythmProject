#include "NoteKeyController.h"

#include <Features/Event/EventManager.h>

#include <Application/EventData/HitKeyData.h>
#include <Application/EventData/ReleaseKeyData.h>


NoteKeyController::NoteKeyController()
{
    EventManager::GetInstance()->AddEventListener("HoldKey", this);
}

NoteKeyController::~NoteKeyController()
{
    EventManager::GetInstance()->AddEventListener("HoldKey", this);
}

void NoteKeyController::Initialize(Stopwatch* _stopWatch, NoteJudge* _noteJudge)
{
    input_ = Input::GetInstance();

    if (_stopWatch == nullptr)        throw std::runtime_error("Stopwatch is not initialized.");
    stopWatch_ = _stopWatch;

    if (_noteJudge == nullptr)        throw std::runtime_error("NoteJudge is not initialized.");
    noteJudge_ = _noteJudge;

    InitializeJsonBinder();

#ifdef _DEBUG
    keySprites_[0].Initialize("key_D");
    keySprites_[1].Initialize("key_F");
    keySprites_[2].Initialize("key_J");
    keySprites_[3].Initialize("key_K");
#endif // _DEBUG

}

void NoteKeyController::Update()
{
    uint32_t laneIndex = 0xffffffff;

    for (uint32_t i = 0; i < laneKeyBindings_.size(); ++i)
    {
        if (input_->IsKeyTriggered(laneKeyBindings_[i]))
        {
            laneIndex = i;
            keySprites_[i].SetColor({1,0,0,1});
            break;
        }

#ifdef _DEBUG
        if (input_->IsKeyReleased(laneKeyBindings_[i]))
        {
            keySprites_[i].SetColor({ 1,1,1,1 });
        }
#endif // _DEBUG
    }

    if (laneIndex != 0xffffffff)
    {
        EventManager::GetInstance()->DispatchEvent(
            GameEvent("HitKey", new HitKeyData(laneIndex, stopWatch_->GetElapsedTime<double>()))
        );
    }

    if (!HoldKey)        return;


    laneIndex = 0xffffffff;

    for (uint32_t i = 0; i < laneKeyBindings_.size(); ++i)
    {
        if (input_->IsKeyReleased(laneKeyBindings_[i]))
        {
            laneIndex = i;
            HoldKey = false;
            break;
        }
    }

    if (laneIndex != 0xffffffff)
    {
        EventManager::GetInstance()->DispatchEvent(
            GameEvent("ReleaseKey", new ReleaseKeyData(laneIndex, stopWatch_->GetElapsedTime<double>()))
        );
    }

}

void NoteKeyController::Draw()
{
#ifdef _DEBUG
    for (auto& keySprite : keySprites_)
    {
        keySprite.Draw();
    }
#endif // _DEBUG
}

void NoteKeyController::OnEvent(const GameEvent& _event)
{
    if (_event.GetEventType() == "HoldKey")
    {
        auto data = static_cast<HitKeyData*>(_event.GetData());
        if (data)
        {
            HoldKey = true;
        }
    }
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
