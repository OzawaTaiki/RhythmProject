#include "MissedVignette.h"

#include <Framework/LayerSystem/LayerSystem.h>

#include <Math/MyLib.h>

void MissedVignette::Initialize()
{
    vignette_ = std::make_unique<Vignette>();
    vignette_->Initialize();

    currentVignetteData_ = VignetteData();
    currentVignetteData_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f); // ビネットの色を赤に設定

    vignette_->SetData(&currentVignetteData_);

    startVignetteData_ = VignetteData();
    startVignetteData_.color = Vector4(1.0f, 0, 0, 1.0f); // ビネットの色を白に設定
    startVignetteData_.power = 0.25f;
    startVignetteData_.scale = 50.0f; // ビネットのスケールを設定
}

void MissedVignette::Update(float _deltaTime)
{
    if(!emit_)
        return;

    elapsedTime_ += _deltaTime;
    if (elapsedTime_ >= duration_)
    {
        emit_ = false; // エフェクトの持続時間が経過したら停止
        currentVignetteData_ = startVignetteData_; // 初期状態に戻す
    }
    else
    {
        // エフェクトの色を徐々に変化させる
        float t = elapsedTime_ / duration_;
        currentVignetteData_.power = Lerp(startVignetteData_.power, 0.0f, t); // パワーを線形補間
    }

}

void MissedVignette::ApplyEffect(const std::string& _input, const std::string& _output)
{
    if (!emit_)
        return;

    LayerSystem::ApplyPostEffect(_input, _output, vignette_.get());

}

void MissedVignette::Emit()
{
    emit_ = true; // エフェクトを発動

    elapsedTime_ = 0.0f; // 経過時間をリセット
    currentVignetteData_ = startVignetteData_; // 初期状態に戻す
}
