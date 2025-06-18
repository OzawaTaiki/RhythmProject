#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>


class TitleScene : public BaseScene
{
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void Initialize(SceneData* _sceneData) override;

    void Update() override;

    void Draw() override;
    void DrawShadow() override;


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
    std::unique_ptr<BeatMapEditor> beatMapEditor_ = nullptr; // ビートマップエディターのインスタンス

};