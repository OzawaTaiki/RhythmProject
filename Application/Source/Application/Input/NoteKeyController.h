#pragma once

// Engine
#include <System/Time/Stopwatch.h>
#include <System/Input/Input.h>
#include <Features/Json/JsonBinder.h>
#include <Features/Event/EventListener.h>
#include <Features/UI/UISprite.h>
#include <System/Audio/VoiceInstance.h>

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/Judge/NoteJudge.h>

// STL
#include <vector>
#include <cstdint>

class NoteKeyController : public iEventListener
{
public:
    NoteKeyController();
    ~NoteKeyController();

    void Initialize( NoteJudge* _noteJudge);
    void Update();
    void Draw();

    void OnEvent(const GameEvent& _event) override;


    /// <summary>
    /// 音楽の音声インスタンスを設定
    /// </summary>
    /// <param name="_voiceInstance">音声インスタンスのポインタ </param>
    void SetMusicVoiceInstance(VoiceInstance* _voiceInstance);
private:

    void InitializeJsonBinder();

private:

    Stopwatch* stopWatch_;          // 時間管理用（所有権なし）
    NoteJudge* noteJudge_;          // 判定用（所有権なし）
    Input* input_;

    VoiceInstance* musicVoiceInstance_; // 音楽の音声インスタンス

    bool holdKey_ = false; // キーを押しているかどうか


    // キー設定
    std::vector<uint32_t> laneKeyBindings_;

    std::unique_ptr<JsonBinder> jsonBinder_;

    /// デバッグ用
    std::array<UISprite, 4> keySprites_;

};