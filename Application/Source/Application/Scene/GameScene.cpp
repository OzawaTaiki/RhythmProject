#include "GameScene.h"
#include <Features/Model/Manager/ModelManager.h>

#include <Features/Model/Primitive/Plane.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Model/Primitive/Ring.h>

#include <Features/Scene/Manager/SceneManager.h>

#include <System/Time/GameTime.h>

#include <Debug/Debug.h>
#include <Debug/ImguITools.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/TextRenderer/TextRenderer.h>

#include <Application/Scene/Transition/SceneTrans.h>
#include <Application/Scene/Data/SceneDatas.h>

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
    GenerateModels();

    std::string beatMapFilePath = "Resources/Data/Game/BeatMap/demo1.json"; // デフォルトの譜面ファイルパス
    gameMode_ = GameMode::Normal;
    if (_sceneData)
    {
        if (_sceneData->beforeScene == "SelectScene")
        {
            auto selectToGameData = dynamic_cast<SelectToGameData*>(_sceneData);
            if (selectToGameData)
            {
                beatMapFilePath = selectToGameData->selectedBeatMapFilePath; // 選択された譜面ファイルパスを取得
                gameMode_ = GameMode::Normal;
            }
        }
        else if (_sceneData->beforeScene == "EditorScene")
        {

            auto editorToGameData = dynamic_cast<SharedBeatMapData*>(_sceneData);
            if (editorToGameData)
            {
                currentBeatMapData_ = editorToGameData->beatMapData; // エディタから渡された譜面データを取得
                if (currentBeatMapData_.title == "None")                    currentBeatMapData_.title = "test";
                gameMode_ = GameMode::EditorTest;
            }
        }
    }

    gameCore_ = std::make_unique<GameCore>(); // レーン数はデフォで4
    gameCore_->Initialize(30.0f, 2.0f); // ノーツの移動速度とオフセット時間を設定

    gameInputManager_ = std::make_unique<GameInputManager>();
    gameInputManager_->Initialize(input_);

    beatMapLoader_ = BeatMapLoader::GetInstance();

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(100);

    feedbackEffect_ = std::make_unique<FeedbackEffect>();
    feedbackEffect_->Initialize(&SceneCamera_);

    gameEnvironment_ = std::make_unique<GameEnvironment>();
    gameEnvironment_->Initiaize();

    gameCore_->SetJudgeCallback([&](int32_t _laneIndex, JudgeType _judgeType) {feedbackEffect_->PlayJudgeEffect(_laneIndex, _judgeType); });


    SceneManager::GetInstance()->SetTransition(std::make_unique<SceneTrans>());

    switch (gameMode_)
    {
    case GameMode::Normal:
        beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap(beatMapFilePath);
        break;
    case GameMode::EditorTest:
        beatMapLoadFuture_ = beatMapLoader_->LoadBeatMap(currentBeatMapData_);
        break;
    default:
        break;
    }

#ifdef _DEBUG
    beatManager_->SetEnableSound(true); // デバッグ時は音を有効にする
#else
    beatManager_->SetEnableSound(false); // デバッグ時以外は音を無効にする
#endif // _DEBUG

    isBeatMapLoaded_ = false;

    isTransitionToResultScene_ = true;
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

#ifdef _DEBUG
    ImGui();

    if(input_->IsKeyTriggered(DIK_F8))
        isTransitionToResultScene_ = !isTransitionToResultScene_; // F8キーで結果シーンへの遷移を切り替え

#endif // _DEBUG

#pragma region Application

    if (input_->IsKeyTriggered(DIK_R))
    {
        voiceInstance_->Stop();
        voiceInstance_.reset();

        voiceInstance_ = soundInstance_->GenerateVoiceInstance(0.3f, 0.0f); // ボリュームとオフセットを設定して再生

        beatManager_->Reset();

        gameCore_->Restart(voiceInstance_);
        gameInputManager_->SetMusicVoiceInstance(voiceInstance_);
        beatManager_->SetMusicVoiceInstance(voiceInstance_);

        isWatingForStart_ = true;
    }



    float deltaTime = static_cast<float>(GameTime::GetInstance()->GetDeltaTime());

    gameInputManager_->Update(); // 入力更新
    beatManager_->Update();
    gameCore_->Update(deltaTime, gameInputManager_->GetInputData());
    feedbackEffect_->Update();
    gameEnvironment_->Update(deltaTime);


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

    if (IsMusicEnd())
    {
        if(isTransitionToResultScene_)
            SceneManager::ReserveScene("ResultScene", nullptr);
    }

    if (gameMode_ == GameMode::EditorTest)
    {
        if (input_->IsKeyTriggered(DIK_ESCAPE))
        {
            auto data = std::make_unique<SharedBeatMapData>();
            data->beatMapData = currentBeatMapData_;
            SceneManager::ReserveScene("EditorScene", std::move(data)); // エディタシーンに譜面データを渡す
        }
    }
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();
    gameCore_->Draw(&SceneCamera_);
    gameEnvironment_->Draw(&SceneCamera_);

    ModelManager::GetInstance()->PreDrawForAlphaObjectModel();


    Sprite::PreDraw();
    //noteKeyController_->Draw();

    feedbackEffect_->Draw();

    particleSystem_->DrawParticles();
}

void GameScene::DrawShadow() {}

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
        gameCore_->GenerateNotes(beatMapLoader_->GetLoadedBeatMapData());

        // bpmを設定
        beatManager_->SetBPM(beatMapLoader_->GetLoadedBeatMapData().bpm);
        beatManager_->SetOffset(beatMapLoader_->GetLoadedBeatMapData().offset);
        std::string audioFilePath = beatMapLoader_->GetLoadedBeatMapData().audioFilePath;
        soundInstance_ = AudioSystem::GetInstance()->Load(audioFilePath);

        gameEnvironment_->SetBPM(beatMapLoader_->GetLoadedBeatMapData().bpm);

        // ロード完了
        Debug::Log("BeatMap Loaded Successfully\n");
        if(soundInstance_)
            voiceInstance_ = soundInstance_->GenerateVoiceInstance(0.3f, 0.0f);
        if (voiceInstance_)
        {
            beatManager_->SetMusicVoiceInstance(voiceInstance_);
            gameCore_->SetMusicVoiceInstance(voiceInstance_);
            gameInputManager_->SetMusicVoiceInstance(voiceInstance_); // 入力管理に音声インスタンスを設定
        }
        else
        {
            Debug::Log("Error: Failed to create voice instance for sound: " + audioFilePath + "\n");
            assert(false);
        }


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
        gameCore_->Start();

        if (voiceInstance_)
            voiceInstance_->Play();
    }
}

bool GameScene::IsMusicEnd() const
{
    if(!isBeatMapLoaded_)
        return false;

    if (!voiceInstance_)
        return false;

    if (!voiceInstance_->IsPlaying())
        return true;

    return false;
}


void GameScene::ImGui()
{
#ifdef _DEBUG

    if (input_->IsKeyTriggered(DIK_SPACE))
        gameEnvironment_->StartAnimation();


    if(ImGuiDebugManager::GetInstance()->Begin("GameScene"))

    {
        if (input_->IsKeyTriggered(DIK_F1))
        {
            enableDebugCamera_ = !enableDebugCamera_;
        }
        float time = voiceInstance_->GetElapsedTime();
        ImGui::Text("Elapsed Time: %.2f", time);


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
            gameEnvironment_->SetBPM(bpm);
        }

        static float volume = 0.2f;
        if (ImGui::DragFloat("music Vol", &volume, 0.01f))
            voiceInstance_->SetVolume(volume);

        static float offset = 0.6f;
        ImGui::DragFloat("offset", &offset, 0.001f);

        if (input_->IsKeyTriggered(DIK_R))
        {
            voiceInstance_->Stop();
            voiceInstance_.reset();
            voiceInstance_ = soundInstance_->Play(volume); // ボリュームとオフセットを設定して再生
            gameCore_->Restart(voiceInstance_); // ゲームコアに音声インスタンスを設定
            gameInputManager_->SetMusicVoiceInstance(voiceInstance_);
            beatManager_->Reset();
        }


        ImGui::Separator();
        ImGui::Text("Music Duration : %.2fsec", soundInstance_->GetDuration());
        if (voiceInstance_)
            ImGui::Text("Music elapse   : %.2fsec", voiceInstance_->GetElapsedTime());

        static float noteSpeed = 30.0f;
        if (ImGui::DragFloat("Note Speed", &noteSpeed, 0.1f, 0.1f, 100.0f))
        {
            gameCore_->SetNoteSpeed(noteSpeed);
        }

        //if (IsMusicEnd())
            //voiceInstance_ = soundInstance_->Play(volume, 130.1f); // スペースキーで音楽を再生



        ImGui::End();
    }

    SceneCamera_.ImGui();



#endif // _DEBUG

}
