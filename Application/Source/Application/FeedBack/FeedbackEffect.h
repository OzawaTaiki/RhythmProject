#pragma once


#include <Application/FeedBack/JudgeSound/JudgeSound.h>
#include <Application/FeedBack/JudgeEffect/JudgeEffect.h>
#include <Application/FeedBack/JudgeText/JudgeText.h>
#include <Application/FeedBack/MissedVignette/MissedVignette.h>

#include <memory>
#include <vector>
#include <list>
#include <bitset>

class Camera;

// フィードバックエフェクト
// 音やパーティクルなど統括する 予定
class FeedbackEffect
{
public:
    FeedbackEffect() = default;
    ~FeedbackEffect() = default;

    void Initialize(Camera* _camera);
    void Update();
    void Draw();

    /// <summary>
    /// ジャッジエフェクトを再生する(コールバック用)
    /// </summary>
    void PlayJudgeEffect(int32_t _laneIndex, JudgeType _judgeType);

    /// <summary>
    /// ミス時のエフェクトを再生する
    /// </summary>
    void PlayMissedEffect();


    void ApplyMissedVignetteEffect(const std::string& _input, const std::string& _output);
private:

    /// <summary>
    /// 判定テキストを割り当てる
    /// </summary>
    /// <param name="_judgeType">判定タイプ</param>
    /// <param name="_position">座標</param>
    void AllocateJudgeText(JudgeType _judgeType, int32_t _laneIndex);


    /// <summary>
    /// デバッグウィンドウを表示する
    /// </summary>
    void DebugWindoow();

private:

    /// 音
    std::unique_ptr<JudgeSound> judgeSound_;

    /// パーティクル
    std::unique_ptr<JudgeEffect> judgeEffect_;

    /// 判定テキスト
    static const int32_t kMaxJudgeTexts_ = 10; // 最大の判定テキスト数
    std::array<std::unique_ptr<JudgeText>, kMaxJudgeTexts_> judgeTextPool_;
    std::bitset<kMaxJudgeTexts_> usedJudgeTexts_; // 使用中のテキストを管理するビットセット


    /// ミス時のエフェクト
    std::unique_ptr<MissedVignette> missedVignette_; // ミス時のビネットエフェクト

    /// UI


    // 座標変換用カメラ
    Camera* camera_ = nullptr; // カメラへのポインタ

};