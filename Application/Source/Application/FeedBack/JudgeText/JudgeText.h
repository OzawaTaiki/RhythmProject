#pragma once

#include <Math/Vector/Vector3.h>
#include <Math/Vector/Vector4.h>
#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/TextRenderer/TextGenerator.h>

#include <Application/Note/Judge/JudgeType.h>

#include <string>

class Camera; // 前方宣言

class JudgeText
{
public:
    // コンストラクタ
    JudgeText();
    // デストラクタ
    ~JudgeText() = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="_judgeType">判定タイプ</param>
    /// <param name="_laneIndex">レーンインデックス</param>
    void Initialize(JudgeType _judgeType, int32_t _laneIndex, const Camera* _camera);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="_deltaTime">デルタタイム</param>
    void Update(float _deltaTime);

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();


    /// <summary>
    /// 描画時間が経過したかどうかを判定
    /// </summary>
    /// <returns>true: 経過した, false: 経過していない</returns>
    bool IsFinished() const { return timer_ >= displayDuration_; }

private:

    /// <summary>
    /// テキストアニメーション処理
    /// </summary>
    void AnimateText();

private:

    /// <summary>
    /// 判定タイプに応じたテキストを取得
    /// </summary>
    /// <param name="_judgeType">判定タイプ</param>
    /// <returns>判定テキスト</returns>
    static std::wstring GetJudgeText(JudgeType _judgeType);

    /// <summary>
    /// 判定タイプに応じたテキストの色を取得
    /// </summary>
    /// <param name="_judgeType">判定タイプ</param>
    /// <param name="_topColor">上頂点の色</param>
    /// <param name="_bottomColor">下頂点の色</param>
    static void GetJudgeTextColor(JudgeType _judgeType, Vector4& _topColor, Vector4& _bottomColor);

    void UpdateTextParam();

    static float displayYOffset_; // Y軸のオフセット
private:

    TextGenerator text_;

    JudgeType judgeType_; // 判定タイプ

    TextParam textParam_; // テキストパラメータ

    Vector4 topColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 上頂点の色
    Vector4 bottomColor_ = Vector4(0.5f, 0.5f, 0.5f, 1.0f); // 下頂点の色

    Vector2 scale_ = { 1.0f, 1.0f }; // テキストのスケール
    Vector2 movement_ = { 0.0f, 0.0f }; // テキストの移動量
    float alpha_ = 1.0f; // テキストのアルファ値

    std::wstring judgeText_; // 判定テキスト

    Vector2 position_;
    float timer_ = 0.0f; // 表示時間のタイマー
    float displayDuration_ = 1.0f; // 表示時間

    std::unique_ptr<AnimationSequence> animationSequence_; // アニメーションシーケンス
};