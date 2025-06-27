#include "FeedbackEffect.h"

void FeedbackEffect::Initialize()
{
    judgeSound_ = std::make_unique<JudgeSound>();
    judgeSound_->Initialize();
}

void FeedbackEffect::Update()
{
    if (judgeSound_)
        judgeSound_->CleanupStoppedVoices(); // 停止した音声をクリーンアップ
}

void FeedbackEffect::Draw()
{
}

void FeedbackEffect::PlayJudgeEffect()
{
    if (judgeSound_)
        judgeSound_->Play();
}
