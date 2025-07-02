#include "FeedbackEffect.h"

#include <Features/Camera/Camera/Camera.h>

void FeedbackEffect::Initialize(Camera* _camera)
{
    if (_camera)
        camera_ = _camera;

    judgeSound_ = std::make_unique<JudgeSound>();
    judgeSound_->Initialize();

    judgeEffect_ = std::make_unique<JudgeEffect>();
    judgeEffect_->Initialize();

    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        judgeTextPool_[i] = std::make_unique<JudgeText>();
        usedJudgeTexts_.set(i, false); // 初期化時は全て未使用
    }
}

void FeedbackEffect::Update()
{
    if (judgeSound_)
        judgeSound_->CleanupStoppedVoices(); // 停止した音声をクリーンアップ

    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (usedJudgeTexts_[i]) // 使用中のテキストのみ更新
        {
            judgeTextPool_[i]->Update(0.016f); // TODO 仮のデルタタイムを使用
            if (judgeTextPool_[i]->IsFinished()) // 終了したテキストは未使用に戻す
            {
                usedJudgeTexts_.set(i, false);
            }
        }
    }
}

void FeedbackEffect::Draw()
{
    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (usedJudgeTexts_[i]) // 使用中のテキストのみ描画
        {
            judgeTextPool_[i]->Draw();
        }
    }
}

void FeedbackEffect::PlayJudgeEffect(int32_t _laneIndex, JudgeType _judgeType)
{

    if (judgeSound_)
        judgeSound_->Play();

    if (judgeEffect_)
        judgeEffect_->Play(_laneIndex);

    AllocateJudgeText(_judgeType, _laneIndex); // 判定テキストを割り当てる
}

void FeedbackEffect::AllocateJudgeText(JudgeType _judgeType, int32_t _laneIndex)
{
    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (!usedJudgeTexts_[i]) // 未使用のテキストを探す
        {
            usedJudgeTexts_.set(i); // 使用中に設定
            judgeTextPool_[i]->Initialize(_judgeType, _laneIndex, camera_);
            return; // 割り当て完了
        }
    }

}
