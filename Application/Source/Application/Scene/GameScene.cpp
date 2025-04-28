#include "GameScene.h"
#include <Features/Model/Manager/ModelManager.h>

GameScene::~GameScene()
{
}

void GameScene::Initialize()
{
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,5,-20 };
    SceneCamera_.rotate_ = { 0.26f,0,0 };
    SceneCamera_.UpdateMatrix();
    debugCamera_.Initialize();


    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->Initialize();
    lineDrawer_->SetCameraPtr(&SceneCamera_);

    input_ = Input::GetInstance();

///---------------------------------
/// Application
///---------------------------------
    stopwatch_ = std::make_unique<Stopwatch>(true, "default");
    stopwatch_->Reset();

    lane_ = std::make_unique<Lane>();
    lane_->Initialize();

    notesSystem_ = std::make_unique<NotesSystem>(lane_.get());
    notesSystem_->Initialize(10.0f, 1.0f);
    notesSystem_->SetStopwatch(stopwatch_.get());

    judgeLine_ = std::make_unique<JudgeLine>();
    judgeLine_->Initialize();


    noteJudge_ = std::make_unique<NoteJudge>();
    noteJudge_->Initialize();
    noteJudge_->SetPosition(judgeLine_->GetPosition());
    noteJudge_->SetLaneTotalWidth(lane_->GetLaneTotalWidth());
    noteJudge_->SetSpeed(notesSystem_->GetNoteSpeed());

    judgeResult_ = std::make_unique<JudgeResult>();
    judgeResult_->Initialize();

    noteKeyController_ = std::make_unique<NoteKeyController>();
    noteKeyController_->Initialize(stopwatch_.get(), noteJudge_.get());

    notesSystem_->SetJudgeLinePosition(judgeLine_->GetPosition());
    notesSystem_->SetMissJudgeThreshold(noteJudge_->GetMissJudgeThreshold());

    stopwatch_->Start();

}

void GameScene::Update()
{
    stopwatch_->Update();

#ifdef _DEBUG
    if (input_->IsKeyTriggered(DIK_F1))
    {
        enableDebugCamera_ = !enableDebugCamera_;
    }
    stopwatch_->ShowDebugWindow();

    judgeResult_->DebugWindow();

#endif // _DEBUG

#pragma region Application

    noteJudge_->SetPosition(judgeLine_->GetPosition());
    noteJudge_->SetLaneTotalWidth(lane_->GetLaneTotalWidth());
    noteJudge_->SetSpeed(notesSystem_->GetNoteSpeed());


    notesSystem_->Update(1.0f / 60.0f);
    lane_->Update();
    noteKeyController_->Update();

#pragma endregion // Application

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
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();

    notesSystem_->DrawNotes(&SceneCamera_);

    lane_->Draw();
    judgeLine_->Draw();
    noteJudge_->DrawJudgeLine();

    Sprite::PreDraw();
    noteKeyController_->Draw();
}

void GameScene::DrawShadow()
{
}
