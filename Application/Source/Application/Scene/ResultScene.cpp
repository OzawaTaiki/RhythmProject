#include "ResultScene.h"

#include <Application/Scene/Data/SceneDatas.h>

void ResultScene::Initialize(SceneData* _sceneData)
{
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,5,-13 };
    SceneCamera_.rotate_ = { 0.26f,0,0 };
    SceneCamera_.UpdateMatrix();
    debugCamera_.Initialize();


    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->Initialize();
    lineDrawer_->SetCameraPtr(&SceneCamera_);

    input_ = Input::GetInstance();

    particleSystem_ = ParticleSystem::GetInstance();
    particleSystem_->SetCamera(&SceneCamera_);

    lightGroup_ = std::make_shared<LightGroup>();
    lightGroup_->Initialize();

    LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);

    // ------------------
    // application

    if (_sceneData)
    {
        auto data = dynamic_cast<GameToResultData*>(_sceneData);
        if (data)
        {
            resultData_ = data->resultData;
        }
    }

    resultUI_ = std::make_unique<ResultUI>();
    resultUI_->Initialize(resultData_);

}

void ResultScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    lightGroup_->ImGui();

#endif // _DEBUG

    resultUI_->Update(static_cast<float>(GameTime::GetInstance()->GetDeltaTime()));


    if (enableDebugCamera_)
    {
        debugCamera_.Update();
        SceneCamera_.matView_ = debugCamera_.matView_;

        SceneCamera_.TransferData();
    }
    else
    {
        SceneCamera_.Update();
        SceneCamera_.UpdateMatrix();
    }

    particleSystem_->Update();



}

void ResultScene::Draw()
{
    resultUI_->Draw();
}

void ResultScene::DrawShadow() {}
