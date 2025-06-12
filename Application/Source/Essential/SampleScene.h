#pragma once
#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>

#include <Features/Effect/Manager/ParticleSystem.h>
#include <System/Time/GameTime.h>
#include <Features/Model/ObjectModel.h>
#include <Features/UI/UIButton.h>
#include <Features/Animation/Sequence/AnimationSequence.h>

#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Effect/Emitter/ParticleEmitter.h>

#include <memory>

class SampleScene : public BaseScene
{
public:

     ~SampleScene() override;

    void Initialize(SceneData* _sceneData) override;
    void Update() override;
    void Draw() override;
    void DrawShadow() override;

private:
    // シーン関連
    Camera SceneCamera_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    std::vector<Particle> particles_;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleManager_ = nullptr;

    std::unique_ptr<ObjectModel> aModel_ = nullptr;

    std::unique_ptr<ObjectModel> oModel_= nullptr;
    std::unique_ptr<ObjectModel> oModel2_= nullptr;
    std::unique_ptr<ObjectModel> plane_ = nullptr;

    std::unique_ptr <Sprite> sprite_ = nullptr;

    std::shared_ptr<LightGroup> lights_;
    std::list<std::pair<float, Vector4>> colors;

    std::unique_ptr<AnimationSequence> sequence_ = nullptr;

    ParticleEmitter emitter_;
    ParticleEmitter emitter_triangle;
    ParticleEmitter emitter_circle;


#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};
