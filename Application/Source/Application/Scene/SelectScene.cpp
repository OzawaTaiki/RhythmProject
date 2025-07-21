#include "SelectScene.h"

#include <Utility/FileDialog/FileDialog.h>
#include <Utility/StringUtils/StringUitls.h>
#include <Features/Scene/Manager/SceneManager.h>
#include <Application/Scene/Data/SceneDatas.h>

void SelectScene::Initialize(SceneData* _sceneData)
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

    text_.Initialize(FontConfig());


    LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);

    selectButton_ = std::make_unique<UIButton>();
    selectButton_->Initialize("SelectButton");
    selectButton_->SetPos({ 640, 360 });
    selectButton_->SetSize({ 200, 100 });
    selectButton_->SetAnchor({ 0.5f,0.5f });
    selectButton_->SetColor({ 0,0,0,1 });

}

void SelectScene::Update()
{
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    //lightGroup_->ImGui();

#endif // _DEBUG

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

    if (selectButton_->IsMousePointerInside())
    {
        if (input_->IsMouseTriggered(0))
        {
            std::string file = FileDialog::OpenFile(FileFilterBuilder::GetFilterString(FileFilterBuilder::FilterType::DataFiles));
            if (!file.empty())
            {
                std::string substr = StringUtils::GetAfterLast(file, "Resources");
                file = "Resources" + substr;

                auto data = std::make_unique<SelectToGameData>();
                data->selectedBeatMapFilePath = file;
                SceneManager::ReserveScene("GameScene", std::move(data));
            }
        }

    }

    TextParam param;
    param.SetColor({ 1,1,1,1 })
        .SetPivot({ 0.5f, 0.5f })
        .SetPosition({ 640, 200 })
        .SetScale({ 1.0f, 1.0f });

    text_.Draw(L"せれくとしーん", param);

    param.SetPosition({ 640, 360 });
    text_.Draw(L"譜面ファイル選択(仮)", param);
}

void SelectScene::Draw()
{

    Sprite::PreDraw();
    selectButton_->Draw();
}

void SelectScene::DrawShadow() {}
