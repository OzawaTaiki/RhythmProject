#include "GameScene.h"
#include <Features/Model/Manager/ModelManager.h>

#include <Features/Model/Primitive/Plane.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Model/Primitive/Ring.h>

#include <System/Time/GameTime.h>
#include <System/Audio/Audio.h>

GameScene::~GameScene()
{
    if (voiceInstance_)
    {
        voiceInstance_->Stop();
        voiceInstance_.reset();
    }
}

// TODO ; やりたいこと にゅうりょく精度アップ
// 別スレッドで入力取得するように。 それと入力された時間を記録

// ロード終了から開始までの待機時間を設定 すぐに始まらんように
void GameScene::Initialize(SceneData* _sceneData)
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

    ///---------------------------------
    /// Application
    ///---------------------------------
    GenerateModels();


    lane_ = std::make_unique<Lane>();
    lane_->Initialize();

    notesSystem_ = std::make_unique<NotesSystem>(lane_.get());
    notesSystem_->Initialize(30.0f, 1.0f);

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
    noteKeyController_->Initialize( noteJudge_.get());

    notesSystem_->SetJudgeLinePosition(judgeLine_->GetPosition());
    notesSystem_->SetMissJudgeThreshold(noteJudge_->GetMissJudgeThreshold());

    beatMapLoader_ = BeatMapLoader::GetInstance();
    beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap("Resources/Data/Game/BeatMap/demo_copy.json");

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100);
    //beatManager_->SetEnableSound(false);

#ifdef _DEBUG
    noteJudge_->SetIsDrawLine(true);
#else
    noteJudge_->SetIsDrawLine(false);
#endif // _DEBUG
    isBeatMapLoaded_ = false;

}

void GameScene::Update()
{

    // ロードが完了してなかったら更新しない
    if (!IsComplateLoadBeatMap())
        return;

    // ごちゃついてきたから少し整理
    /*
        isWating は曲を再生するまでの待機状態を表すフラグ
        これが真のときは noteは動いていてほしい 描画もしてくれ
        負のときは？ それはすでにゲームが始まっている状態 なにも制限する必要はない
        -> 特に制限する必要はなく 時間がたったら音楽を再生するだけ
        なので、ゲーム開始オフセット時間を超えたら falseにする(必要ある?wkrn)

    */

    UpdateGameStartOffset();

    ImGui();

#pragma region Application

    beatManager_->Update();
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


    Plane plane_py0n1;
    plane_py0n1.SetSize(Vector2(1.0f, 1.0f) * 2);
    plane_py0n1.SetNormal(Vector3(0, 0, -1));
    plane_py0n1.SetPivot(Vector3(0, -1, 0));
    plane_py0n1.SetFlipV(true);

    plane_py0n1.Generate("plane_py0n1");

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
        beatManager_->SetOffset(beatMapLoader_->GetLoadedBeatMapData().offset);
        std::string audioFilePath = beatMapLoader_->GetLoadedBeatMapData().audioFilePath;
        soundInstance_ = AudioSystem::GetInstance()->Load(audioFilePath);


        // ロード完了
        Debug::Log("BeatMap Loaded Successfully\n");
        if(soundInstance_)
            voiceInstance_ = soundInstance_->GenerateVoiceInstance(0.3f);
        if (voiceInstance_)
        {
            beatManager_->SetMusicVoiceInstance(voiceInstance_);
            notesSystem_->SetMusicVoiceInstance(voiceInstance_);
            noteKeyController_->SetMusicVoiceInstance(voiceInstance_.get());
        }
        else
        {
            Debug::Log("Error: Failed to create voice instance for sound: " + audioFilePath + "\n");
            assert(false);
        }


        notesSystem_->playing(true); // noteはながれてほしいから
        isBeatMapLoaded_ = true;
        isWatingForStart_ = true; // 譜面読み込み完了したら開始待機状態にする
    }

    return isBeatMapLoaded_;
}

void GameScene::UpdateGameStartOffset()
{
    if (!isWatingForStart_)
        return;

    // 時間になったら 音楽等再生 trueを返す
        // タイマー更新
    waitTimer_ += static_cast<float>(GameTime::GetInstance()->GetDeltaTime());
    // ゲーム開始オフセット時間を超えたら
    if (waitTimer_ >= gameStartOffset_)
    {
        isWatingForStart_ = false;
        waitTimer_ = 0.0f;
        // ゲーム開始
        beatManager_->Start();
        notesSystem_->Start();
        if (voiceInstance_)
            voiceInstance_->Play();
    }
}

void GameScene::ImGui()
{
#ifdef _DEBUG
    if (input_->IsKeyTriggered(DIK_F1))
    {
        enableDebugCamera_ = !enableDebugCamera_;
    }
    float time = voiceInstance_->GetElapsedTime();
    ImGui::Text("Elapsed Time: %.2f", time);

    if (input_->IsKeyTriggered(DIK_SPACE))
    {
        float time = voiceInstance_->GetElapsedTime();
        Debug::Log("\n\nVoiceInstance Elapsed Time: " + std::to_string(time) + "\n\n");
    }

    judgeResult_->DebugWindow();

    if (ImGui::Button("stop"))
        beatManager_->Stop();
    if (ImGui::Button("play##beat"))
        beatManager_->Start();


    if (ImGui::Button("play##music"))
    {
        voiceInstance_->Play();
    }
    static float bpm = 120;
    ImGui::DragFloat("BPM", &bpm, 0.1f);
    if (ImGui::Button("SetBPM"))
    {
        beatManager_->SetBPM(bpm);
    }

    static float volume = 0.2f;
    if (ImGui::DragFloat("music Vol", &volume, 0.01f))
        voiceInstance_->SetVolume(volume);

    static float offset = 0.6f;
    ImGui::DragFloat("offset", &offset, 0.001f);

    if (input_->IsKeyTriggered(DIK_R))
    {
        beatManager_->SetOffset(offset);
        voiceInstance_->Stop();
        voiceInstance_.reset();
        voiceInstance_ = soundInstance_->Play(volume); // ボリュームとオフセットを設定して再生
        notesSystem_->SetMusicVoiceInstance(voiceInstance_);
        beatManager_->Reset();
        notesSystem_->Reload();
    }


    noteJudge_->SetPosition(judgeLine_->GetPosition());
    noteJudge_->SetLaneTotalWidth(lane_->GetLaneTotalWidth());
    noteJudge_->SetSpeed(notesSystem_->GetNoteSpeed());

#endif // _DEBUG

}
