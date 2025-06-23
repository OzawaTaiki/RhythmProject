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

    stopWatch_ = nullptr;
    musicVoiceInstance_ = nullptr;

}

void NoteKeyController::Initialize( NoteJudge* _noteJudge)
{
    input_ = Input::GetInstance();

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
    if (musicVoiceInstance_ == nullptr)
    {
        throw std::runtime_error("Music voice instance is not set.");
        return;
    }

    uint32_t laneIndex = 0xffffffff;

    for (uint32_t i = 0; i < laneKeyBindings_.size(); ++i)
    {
        if (input_->IsKeyTriggered(laneKeyBindings_[i]))
        {
            laneIndex = i;
            keySprites_[i].SetColor({1,0,0,1});
            // TODO : 同時押し対応 vector等で
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
        // イベント発行          To Lane.cpp
        EventManager::GetInstance()->DispatchEvent(
            GameEvent("HitKey", new HitKeyData(laneIndex, musicVoiceInstance_->GetElapsedTime()))
        );
    }

    if (!holdKey_)        return;


    laneIndex = 0xffffffff;

    for (uint32_t i = 0; i < laneKeyBindings_.size(); ++i)
    {
        if (input_->IsKeyReleased(laneKeyBindings_[i]))
        {
            laneIndex = i;
            holdKey_ = false;
            break;
        }
    }

    if (laneIndex != 0xffffffff)
    {
        // イベント発行          To Lane.cpp
        EventManager::GetInstance()->DispatchEvent(
            GameEvent("ReleaseKey", new ReleaseKeyData(laneIndex, musicVoiceInstance_->GetElapsedTime()))
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
            holdKey_ = true;
        }
    }
}

void NoteKeyController::SetMusicVoiceInstance(VoiceInstance* _voiceInstance)
{
    // nullチェック
    if (_voiceInstance == nullptr)
        throw std::runtime_error("VoiceInstance is not initialized.");

    musicVoiceInstance_ = _voiceInstance;
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
