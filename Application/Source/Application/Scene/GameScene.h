#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <Features/Sprite/Sprite.h>
#include <System/Audio/AudioSystem.h>


// Application
#include <Application/Core/GameCore.h>
#include <Application/Input/GameInputManager.h>
#include <Application/FeedBack/FeedbackEffect.h>

#include <Application/BeatsManager/BeatManager.h>
#include <Application/BeatMapLoader/BeatMapLoader.h>


class GameScene : public BaseScene
{
public:
    ~GameScene() override;
    void Initialize(SceneData* _sceneData) override;
    void Update() override;
    void Draw() override;
    void DrawShadow() override;

private:

    void GenerateModels();

    /// <summary>
    /// 譜面データの読み込みを待機する
    /// </summary>
    /// <param name="_filePath">譜面データのファイルパス</param>
    /// <returns> 読み込み完了 or 読み込み済みなら true </returns>
    bool IsComplateLoadBeatMap();

    /// <summary>
    /// ゲーム開始オフセット処理の更新
    /// </summary>
    void UpdateGameStartOffset();


    // 曲の再生が終わったか
    bool IsMusicEnd() const;

private:

    // シーン関連
    Camera SceneCamera_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

    // Application

    std::unique_ptr<GameCore> gameCore_ = nullptr;                  // ゲームの核となる部分
    std::unique_ptr<GameInputManager> gameInputManager_ = nullptr;  // ゲームの入力管理
    std::unique_ptr<FeedbackEffect> feedbackEffect_ = nullptr;      // フィードバックエフェクト

    BeatMapLoader* beatMapLoader_ = nullptr;

    //std::unique_ptr<Stopwatch> stopwatch_ = nullptr;

    std::unique_ptr<BeatManager> beatManager_ = nullptr;

    std::future<bool> beatMapLoadFuture_ = {};

    bool isBeatMapLoaded_ = false;

    bool isWatingForStart_ = false;
    float gameStartOffset_ = 2.0f;
    float waitTimer_ = 0.0f;

    std::shared_ptr<SoundInstance> soundInstance_ = nullptr;
    std::shared_ptr<VoiceInstance> voiceInstance_ = nullptr;

    void ImGui();


};