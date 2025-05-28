#include "LaneEffect.h"


void LaneEffect::Initialize(const Vector3& _laneCenter, Model* _planeModel)
{

    auto model = std::make_unique<ObjectModel>("laneEffectPlane");
    model->Initialize(_planeModel);
    model->translate_ = _laneCenter;
    model->translate_.y -= 0.05f;

    laneModel_ = std::move(model);

    duration_ = 1.0f; // デフォルトの持続時間を設定
    timer_ = 0.0f; // タイマーをリセット
    isActive_ = false; // エフェクトをアクティブに設定
    color_ = { 0.5f, 0.5f, 0.5f, 1.0f }; // デフォルトの色を設定
}

void LaneEffect::Update(float _deltaTime)
{
    if (!isActive_)
    {
        return; // エフェクトがアクティブでない場合は何もしない
    }

    timer_ += _deltaTime;
    if (timer_ > duration_)
    {
        timer_ = duration_; // リセット
        End();
    }

    float progress = timer_ / duration_;

    color_.w = 1.0f - progress; // フェードアウト効果

    laneModel_->Update();
}

void LaneEffect::Draw(const Camera* _camera)
{
    if (!isActive_)
    {
        return; // エフェクトがアクティブでない場合は描画しない
    }

    // laneModels_の各モデルを描画
    if (laneModel_)
    {
        laneModel_->Draw(_camera, color_);
    }
}

void LaneEffect::Start()
{
    isActive_ = true;
    timer_ = 0.0f; // タイマーをリセット
}

void LaneEffect::End()
{
    isActive_ = false; // エフェクトを非アクティブに設定
    timer_ = 0.0f; // タイマーをリセット
}
