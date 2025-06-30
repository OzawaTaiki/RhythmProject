#include "FeedbackEffect.h"

void FeedbackEffect::Initialize()
{
    judgeSound_ = std::make_unique<JudgeSound>();
    judgeSound_->Initialize();

    judgeEffect_ = std::make_unique<JudgeEffect>();
    judgeEffect_->Initialize();
}

void FeedbackEffect::Update()
{
    if (judgeSound_)
        judgeSound_->CleanupStoppedVoices(); // 停止した音声をクリーンアップ
}

void FeedbackEffect::Draw()
{
}

void FeedbackEffect::PlayJudgeEffect(int32_t _laneIndex)
{
    if (judgeSound_)
        judgeSound_->Play();

    if (judgeEffect_)
        judgeEffect_->Play(_laneIndex);
}
