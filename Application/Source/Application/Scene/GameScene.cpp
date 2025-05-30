#include "GameScene.h"
#include <Features/Model/Manager/ModelManager.h>

#include <Features/Model/Primitive/Plane.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Model/Primitive/Ring.h>

#include <System/Time/GameTime.h>
#include <System/Audio/Audio.h>

GameScene::~GameScene()
{
}

void GameScene::Initialize(SceneData* _sceneData)
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

    particleSystem_ = ParticleSystem::GetInstance();
    particleSystem_->SetCamera(&SceneCamera_);

    lightGroup_ = std::make_shared<LightGroup>();
    lightGroup_->Initialize();


    LightingSystem::GetInstance()->SetActiveGroup(lightGroup_);

    ///---------------------------------
    /// Application
    ///---------------------------------
    GenerateModels();

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

    beatMapLoader_ = BeatMapLoader::GetInstance();
    beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap("Resources/Data/Game/BeatMap/Luminous_Memory.json");

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100.0f);
    beatManager_->SetStopWatch(stopwatch_.get());
    beatManager_->SetEnableSound(false);

    isBeatMapLoaded_ = false;
    frameCount_ = 0;

}

void GameScene::Update()
{

    // ロードが完了してなかったら更新しない
    if (!IsComplateLoadBeatMap())
        return;

    stopwatch_->Update();

#ifdef _DEBUG
    if (input_->IsKeyTriggered(DIK_F1))
    {
        enableDebugCamera_ = !enableDebugCamera_;
    }
    stopwatch_->ShowDebugWindow();

    judgeResult_->DebugWindow();

    if(ImGui::Button("stop"))
        beatManager_->Stop();
    if (ImGui::Button("play"))
        beatManager_->Start();

    static float bpm = 120;
    ImGui::DragFloat("BPM", &bpm, 0.1f);
    if (ImGui::Button("SetBPM"))
    {
        beatManager_->SetBPM(bpm);
    }

    noteJudge_->SetPosition(judgeLine_->GetPosition());
    noteJudge_->SetLaneTotalWidth(lane_->GetLaneTotalWidth());
    noteJudge_->SetSpeed(notesSystem_->GetNoteSpeed());

#endif // _DEBUG
    beatManager_->Update();

    if(Input::GetInstance()->IsKeyTriggered(DIK_F2))
        beatManager_->SetBPM(100);


#pragma region Application

    notesSystem_->Update(static_cast<float> (GameTime::GetInstance()->GetDeltaTime()));
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

    particleSystem_->Update();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();

    judgeLine_->Draw();
    noteJudge_->DrawJudgeLine();
    notesSystem_->DrawNotes(&SceneCamera_);

    ModelManager::GetInstance()->PreDrawForAlphaObjectModel();
    lane_->Draw(&SceneCamera_);


    Sprite::PreDraw();
    //noteKeyController_->Draw();

    particleSystem_->DrawParticles();
}

void GameScene::DrawShadow()
{
}

void GameScene::GenerateModels()
{// 2x2 y+向き
    Plane plane;
    plane.SetSize(Vector2(1.0f, 1.0f) * 2);
    plane.SetNormal(Vector3(0, 1, 0));
    plane.SetPivot(Vector3(0, 0, 0));

    plane.Generate("pY1x1Plane");

    Plane plane_py01;
    plane_py01.SetSize(Vector2(1.0f, 1.0f));
    plane_py01.SetNormal(Vector3(0, 1, 0));
    plane_py01.SetPivot(Vector3(0, 1, 0));

    plane_py01.Generate("pY1x1p01Plane");// y+向き 1x1 pivot(0,1,0)

    // ほそ長いやつ
    Plane plane2;
    plane2.SetSize(Vector2(0.1f, 0.7f) * 5.0f);
    plane2.SetNormal(Vector3(0, 0, -1));
    plane2.SetPivot(Vector3(0, 0, 0));

    plane2.Generate("nZ0.1x0.7Plane");


    Triangle triangle;
    triangle.SetNormal(Vector3(0, 0, -1));
    triangle.SetVertices({
        Vector3(0, 0.5f, 0),
        Vector3(0.5f, -0.5f, 0),
        Vector3(-0.5f, -0.5f, 0)
        });
    triangle.Generate("nZ1_1Triangle");
}

bool GameScene::IsComplateLoadBeatMap()
{
    if (beatMapLoader_->IsLoading())
    {
        Debug::Log("Loading BeatMap...\n");
    }
    else if (!isBeatMapLoaded_)
    {
        if (!beatMapLoader_->IsLoadingSuccess())
        {
            // 読み込み失敗してたら エラーメッセージを取得して表示
            std::string errorMessage = beatMapLoader_->GetErrorMessage();
            Debug::Log("Error: " + errorMessage + "\n");
            assert(false);
        }

        // 譜面データを渡してnoteを生成
        notesSystem_->SetBeatMapDataAndCreateNotes(beatMapLoader_->GetLoadedBeatMapData());

        // bpmを設定
        static float bpm = beatMapLoader_->GetLoadedBeatMapData().bpm;
        beatManager_->SetBPM(bpm);

        std::string audioFilePath = beatMapLoader_->GetLoadedBeatMapData().audioFilePath;
        uint32_t handle = Audio::GetInstance()->SoundLoadWave(audioFilePath);

        // ロード完了
        Debug::Log("BeatMap Loaded Successfully\n");

        // 開始する
        beatManager_->Start();
        stopwatch_->Start();

        isBeatMapLoaded_ = true;
    }

    return isBeatMapLoaded_;
}
