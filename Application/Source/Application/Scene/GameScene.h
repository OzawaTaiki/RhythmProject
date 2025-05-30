#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/NotesSystem.h>
#include <Application/Note/Judge/JudgeLine.h>
#include <Application/Note/Judge/NoteJudge.h>
#include <Application/Note/Judge/JudgeResult.h>
#include <Application/Input/NoteKeyController.h>
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

    // シーン関連
    Camera SceneCamera_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

    // Application
    std::unique_ptr<Lane> lane_ = nullptr;
    std::unique_ptr<NotesSystem> notesSystem_ = nullptr;
    std::unique_ptr<JudgeLine> judgeLine_ = nullptr;
    std::unique_ptr<NoteJudge> noteJudge_ = nullptr;
    std::unique_ptr<JudgeResult> judgeResult_ = nullptr;
    std::unique_ptr<NoteKeyController> noteKeyController_ = nullptr;

    BeatMapLoader* beatMapLoader_ = nullptr;

    std::unique_ptr<Stopwatch> stopwatch_ = nullptr;

    std::unique_ptr<BeatManager> beatManager_ = nullptr;

    std::future<bool> beatMapLoadFuture_ = {};

    bool isBeatMapLoaded_ = false;
    int frameCount_ = 0;
};