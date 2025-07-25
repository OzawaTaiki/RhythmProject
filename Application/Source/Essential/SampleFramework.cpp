#include "SampleFramework.h"

#include "SampleScene.h"
#include <Features/Scene/ParticleTestScene.h>
#include "SceneFactory.h"

#include <System/Time/Time_MT.h>
#include <Essential/ParticleModifierFactory.h>

#include <Framework/LayerSystem/LayerSystem.h>

void SampleFramework::Initialize(const std::wstring& _winTitle)
{
    Framework::Initialize(L"GameEngine");


    //rtvManager_->CreateRenderTarget("default", WinApp::kWindowWidth_, WinApp::kWindowHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(0.4625f, 0.925f, 0.4625f, 1.0f), false);
    rtvManager_->CreateRenderTarget("default", WinApp::kWindowWidth_, WinApp::kWindowHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(0.0190f, 0.0190f, 0.0933f, 1.0f), false);
    rtvManager_->CreateRenderTarget("ShadowMap", 4096, 4096, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,  Vector4(1.0f, 1.0f, 1.0f, 1.0f),true);

    sceneManager_->SetSceneFactory(new SceneFactory());

    particleManager_->SetModifierFactory(new ParticleModifierFactory());

    collisionManager_->Initialize(Vector2(100, 100), 5, Vector2(-50, -50), 1.0f);

    Time_MT::GetInstance()->Initialize();

    LayerSystem::Initialize();

    // 最初のシーンで初期化
    sceneManager_->Initialize("Sample");
}

void SampleFramework::Update()
{
    Framework::Update();

    rtvManager_->ClearAllRenderTarget();


    //========== 更新処理 =========

    sceneManager_->Update();
    //particleManager_->Update(); TODO ; 引数のカメラの回転をなんとかしたい

    //=============================
}

void SampleFramework::Draw()
{
    Framework::PreDraw();

    rtvManager_->SetDepthStencil("ShadowMap");
    sceneManager_->DrawShadow();

    rtvManager_->SetRenderTexture("default");

    // ========== 描画処理 ==========

    sceneManager_->Draw();

    lineDrawer_->Draw();
    //=============================

    textRenderer_->EndFrame();

    PSOManager::GetInstance()->SetPipeLineStateObject(PSOFlags::Type::Composite);
    PSOManager::GetInstance()->SetRootSignature(PSOFlags::Type::Composite);

    LayerSystem::CompositeAllLayers("default");
    particleManager_->DrawParticles();

    dxCommon_->PreDraw();
    // スワップチェインに戻す
    rtvManager_->SetSwapChainRenderTexture(dxCommon_->GetSwapChain());

    PSOManager::GetInstance()->SetPipeLineStateObject(PSOFlags::Type::OffScreen);
    PSOManager::GetInstance()->SetRootSignature(PSOFlags::Type::OffScreen);

    // レンダーテクスチャを描画
    rtvManager_->DrawRenderTexture("default");

    Framework::PostDraw();

    // 後にupdateに
    sceneManager_->ChangeScene();


}

void SampleFramework::Finalize()
{
    Time_MT::GetInstance()->Finalize();
    Framework::Finalize();
}
