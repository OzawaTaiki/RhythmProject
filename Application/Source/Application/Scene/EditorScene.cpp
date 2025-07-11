#include "EditorScene.h"

#include <Features/Scene/Manager/SceneManager.h>

#include <Application/Scene/Data/SceneDatas.h>

void EditorScene::Initialize(SceneData* _sceneData)
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


    //---------------------------------
    // Application
    //---------------------------------


    if (_sceneData)
    {
        if (_sceneData->beforeScene == "GameScene")
        {
            beatMapEditor_ = std::make_unique<BeatMapEditor>();
            auto gameToEditorData = dynamic_cast<SharedBeatMapData*>(_sceneData);
            if (gameToEditorData)
                beatMapEditor_->Initialize(gameToEditorData->beatMapData); // ゲームから渡された譜面データを取得
        }
    }

    if (!beatMapEditor_)
    {
        beatMapEditor_ = std::make_unique<BeatMapEditor>();
        beatMapEditor_->Initialize(); // 初期化
    }
}

void EditorScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

#endif // _DEBUG


    /// ---------------------------------
    /// Application
    {
        beatMapEditor_->Update();

        if (beatMapEditor_->ToTestMode())
        {
            auto data = std::make_unique<SharedBeatMapData>();
            data->beatMapData = beatMapEditor_->GetBeatMapData(); // エディタから譜面データを取得
            SceneManager::ReserveScene("GameScene", std::move(data)); // ゲームシーンに譜面データを渡す
        }

    }
    /// Application ここまで
    /// -----------------------------------



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

void EditorScene::Draw()
{
    beatMapEditor_->Draw(&SceneCamera_);
}

void EditorScene::DrawShadow() {}
