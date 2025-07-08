#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <Features/TextRenderer/TextRenderer.h>

#include <Features/UI/UIButton.h>

class SelectScene : public BaseScene
{
public:
    SelectScene() = default;
    ~SelectScene() override = default;

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

    TextRenderer* textRenderer_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

    std::unique_ptr<UIButton> selectButton_ = nullptr;

};