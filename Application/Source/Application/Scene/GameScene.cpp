#include "GameScene.h"
#include <Features/Model/Manager/ModelManager.h>

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

    if (beatMapLoader_->IsLoading())
    {
        Debug::Log("Loading BeatMap...\n");
        return;
    }
    else if(!isBeatMapLoaded_)
    {
        if (!beatMapLoader_->IsLoadingSuccess())
        {
            std::string errorMessage = beatMapLoader_->GetErrorMessage();
            Debug::Log("Error: " + errorMessage+"\n");
            return;
        }

        notesSystem_->SetBeatMapDataAndCreateNotes(beatMapLoader_->GetLoadedBeatMapData());

        static float bpm = beatMapLoader_->GetLoadedBeatMapData().bpm;
        beatManager_->SetBPM(bpm);

        std::string audioFilePath = beatMapLoader_->GetLoadedBeatMapData().audioFilePath;
        uint32_t handle = Audio::GetInstance()->SoundLoadWave(audioFilePath);

        //Audio::GetInstance()->SoundPlay(handle, 0.5f, true, false);

        Debug::Log("BeatMap Loaded Successfully\n");
        beatManager_->Start();
        stopwatch_->Start();

        isBeatMapLoaded_ = true;
        return;
    }
    stopwatch_->Update();

    //Debug::Log("Begin Frame : " + std::to_string(stopwatch_->GetElapsedTime<float>()) + "\n");
    //Debug::Log("Frame Count : " + std::to_string(frameCount_++) + "\n");



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

    //notesSystem_->Update(static_cast<float> (GameTime::GetInstance()->GetDeltaTime()));
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

    judgeLine_->Draw();
    noteJudge_->DrawJudgeLine();

    lane_->Draw(&SceneCamera_);


    Sprite::PreDraw();
    //noteKeyController_->Draw();
}

void GameScene::DrawShadow()
{
}
