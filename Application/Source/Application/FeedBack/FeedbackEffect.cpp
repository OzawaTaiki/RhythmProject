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

    // ミス時のビネットエフェクトの初期化
    missedVignette_ = std::make_unique<MissedVignette>();
    missedVignette_->Initialize();

}

void FeedbackEffect::Update()
{
    //DebugWindoow(); // デバッグウィンドウの更新

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

    if (missedVignette_)
        missedVignette_->Update(0.016f); // TODO 仮のデルタタイムを使用
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

void FeedbackEffect::PlayMissedEffect()
{
    if (missedVignette_)
    {
        missedVignette_->Emit(); // ミス時のビネットエフェクトを発動
    }
}

void FeedbackEffect::ApplyMissedVignetteEffect(const std::string& _input, const std::string& _output)
{
    if (missedVignette_)
    {
        missedVignette_->ApplyEffect(_input, _output); // ビネットエフェクトを適用
    }
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

void FeedbackEffect::DebugWindoow()
{
#ifdef _DEBUG

    ImGui::Begin("FeedbackEffect Debug");
    {
        ImGui::SeparatorText("JudgeText");
        if (ImGui::Button("Perfect"))
            AllocateJudgeText(JudgeType::Perfect, 0);
        ImGui::SameLine();
        if (ImGui::Button("Good"))
            AllocateJudgeText(JudgeType::Good, 1);
        ImGui::SameLine();
        if (ImGui::Button("Miss"))
            AllocateJudgeText(JudgeType::Miss, 2);

    }
    ImGui::End();

#endif
}
