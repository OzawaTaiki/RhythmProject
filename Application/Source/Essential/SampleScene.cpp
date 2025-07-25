#include "SampleScene.h"

#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Core/DXCommon/RTV/RTVManager.h>

#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Sprite/Sprite.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Model/Primitive/Plane.h>
#include <Features/Collision/Manager/CollisionManager.h>



#include <Debug/ImGuiManager.h>
#include <Framework/LayerSystem/LayerSystem.h>


SampleScene::~SampleScene()
{
}

void SampleScene::Initialize(SceneData* _sceneData)
{

    // --------------------------------------------------
    // シーン関連 初期化
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,5,-20 };
    SceneCamera_.rotate_ = { 0.26f,0,0 };
    SceneCamera_.UpdateMatrix();
    debugCamera_.Initialize();


    lineDrawer_ = LineDrawer::GetInstance();
    //lineDrawer_->Initialize();
    lineDrawer_->SetCameraPtr(&SceneCamera_);

    input_ = Input::GetInstance();

    // パーティクルシステムの初期化
    // カメラのポイントを設定する
    ParticleSystem::GetInstance()->SetCamera(&SceneCamera_);


    // ライトの設定
    // ライトグループの初期化 (関数内でDLは初期化される)
    lights_ = std::make_shared<LightGroup>();
    lights_->Initialize();

    // DLを取得して初期化 (任意)
    auto DL = lights_->GetDirectionalLight();
    DL->SetDirection(Vector3(-1.0f, -1.0f, 0.0f).Normalize());

    // ライトの追加(任意)
    // 追加するライトを初期化する
    std::shared_ptr<PointLightComponent> pointLight = std::make_shared<PointLightComponent>();
    pointLight->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    pointLight->SetIntensity(1.0f);
    pointLight->SetRadius(30.0f);
    pointLight->SetPosition({ 0.0f, 5.0f, 0.0f });

    // ライトグループにポイントライトを追加する
    lights_->AddPointLight("pointlight", pointLight);

    // アクティブなライトグループを設定する
    LightingSystem::GetInstance()->SetActiveGroup(lights_);



    // --------------------------------------------------
    // シーン固有の初期化

    human_ = std::make_unique<ObjectModel>("human");
    // モデルのを読み込む
    human_->Initialize("human/walk.gltf");
    // アニメーション読み込み
    // 任意の名前を設定できる
    human_->LoadAnimation("human/walk.gltf", "walk");

    bool loop = true;
    // アニメーションを再生する
    human_->SetAnimation("walk", loop);


    // 地面ようのいたポリを生成する
    Plane groundPlane;
    groundPlane.SetSize(Vector2{ 100, 100 });   //サイズは100*100
    groundPlane.SetPivot({ 0,0,0 });        // 基準点は中央
    groundPlane.SetNormal({ 0,1,0 });       //上向き
    groundPlane.SetFlipU(false);            // U軸の反転はなし
    groundPlane.SetFlipV(false);            // V軸の反転はなし

    // 任意の名前で生成する
    groundPlane.Generate("groundPlane");

    // 地面を生成する
    ground_ = std::make_unique<ObjectModel>("ground");
    // 生成した板ポリを使用する (生成時に設定した名前を渡す)
    ground_->Initialize("terrain.obj");
    // UV変換を設定する
    //ground_->GetUVTransform().SetScale({ 10,10 });

    // 地面のテクスチャを読み込む 描画時に使用する
    groundTextureHandle_ = TextureManager::GetInstance()->Load("white.png");


    // 2Dスプライトの初期化
    uint32_t textureHandle = TextureManager::GetInstance()->Load("uvChecker.png");
    sprite_ = Sprite::Create("uvChecker", textureHandle);


    // 音声データの読み込み
    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/Alarm01.wav");

    //skyBox_ = std::make_unique<SkyBox>();
    //skyBox_->Initialize(30.0f);
    //skyBox_->SetTexture("rosendal_plains_2_2k.dds");

    emitter_ = std::make_unique<ParticleEmitter>();
    emitter_->Initialize("TapEffect_01");



    grayScale_ = std::make_unique<GrayScale>();
    grayScale_->Initialize();

    grayScaleData_.intensity = 1.0f;

    grayScale_->SetData(&grayScaleData_);


    LayerSystem::CreateLayer("Model", 0);
    LayerSystem::CreateLayer("Main", 1);


    textGenerator_.Initialize(FontConfig());


}

void SampleScene::Update()
{
    // シーン関連更新
#ifdef _DEBUG

    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;


    {
        ImGui::Begin("Engine");
        {
            // サウンドの再生
            if (ImGui::Button("play Sound"))
            {
                if (soundInstance_)
                {
                    // 返り値で VoiceInstanceを受け取る
                    // これを使用して音量やピッチの変更ができる
                    voiceInstance_ = soundInstance_->Play(0.3f);
                }
            }

            static float volume = 1.0f;
            if (ImGui::DragFloat("Volume", &volume, 0.0f, 1.0f))
            {
                if (voiceInstance_)
                {
                    voiceInstance_->SetVolume(volume);
                }
            }
            if (ImGui::Button("Stop Sound"))
            {
                if (voiceInstance_)
                {
                    voiceInstance_->Stop();
                    voiceInstance_ = nullptr; // VoiceInstanceを解放
                }
            }


        }
        ImGui::End();

        // light調整用
        lights_->ImGui();

    }
    ImGui::Begin("Emitter");
    emitter_->ShowDebugWindow();
    ImGui::End();

#endif // _DEBUG

    if (input_->IsKeyTriggered(DIK_SPACE))
    {
        // シーンの切り替え
        SceneManager::ReserveScene("GameScene",nullptr);
    }

    if (input_->IsKeyPressed(DIK_LEFT))
        grayScaleData_.intensity -= 0.01f;
    if (input_->IsKeyPressed(DIK_RIGHT))
        grayScaleData_.intensity += 0.01f;

    grayScaleData_.intensity = std::clamp(grayScaleData_.intensity, 0.0f, 1.0f);

    // モデルの更新
    human_->Update();
    ground_->Update();
    emitter_->Update(0.016f);


    textGenerator_.Draw(L"← → でグレースケールの強度変化", Vector2(200, 300), Vector4(1, 0, 0, 1));
    textGenerator_.Draw(L"Space でシーンチェンジ", Vector2(200, 500), Vector4(1, 0, 0, 1));
    // --------------------------------
    // シーン共通更新処理

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

    ParticleSystem::GetInstance()->Update();
    CollisionManager::GetInstance()->Update();
}

void SampleScene::Draw()
{
    // skyBooxの描画
    //skyBox_->Draw(&SceneCamera_);

    // Model描画用のPSO等をセット
    ModelManager::GetInstance()->PreDrawForObjectModel();

    // SkyBoxのキューブマップを描画キューに追加(任意)
    //skyBox_->QueueCmdCubeTexture();
    LayerSystem::SetLayer("Model");
    // groundの描画
    ground_->Draw(&SceneCamera_,  drawColor_);
    // humanの描画
    human_->Draw(&SceneCamera_, drawColor_);

    //LayerSystem::ApplyPostEffect("Model", "BoxFilter", boxFilter_);

    // Sprite用のPSO等をセット
    Sprite::PreDraw();
    // スプライトの描画
    sprite_->Draw(Vector4(1, 1, 1, 1));
    LayerSystem::ApplyPostEffect("Model", "Main", grayScale_.get());


    ParticleSystem::GetInstance()->DrawParticles();

}

void SampleScene::DrawShadow()
{
    human_->DrawShadow(&SceneCamera_);
}

#ifdef _DEBUG
void SampleScene::ImGui()
{

}
#endif // _DEBUG
