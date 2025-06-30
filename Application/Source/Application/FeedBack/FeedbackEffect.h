#pragma once

#include <Application/FeedBack/JudgeSound/JudgeSound.h>
#include <Application/FeedBack/JudgeEffect/JudgeEffect.h>

#include <memory>

// フィードバックエフェクト
// 音やパーティクルなど統括する 予定
class FeedbackEffect
{
public:
    FeedbackEffect() = default;
    ~FeedbackEffect() = default;

    void Initialize();
    void Update();
    void Draw();

    /// <summary>
    /// ジャッジエフェクトを再生する(コールバック用)
    /// </summary>
    void PlayJudgeEffect(int32_t _laneIndex);

private:

    /// 音
    std::unique_ptr<JudgeSound> judgeSound_;

    /// パーティクル
    std::unique_ptr<JudgeEffect> judgeEffect_;

    /// UI

};