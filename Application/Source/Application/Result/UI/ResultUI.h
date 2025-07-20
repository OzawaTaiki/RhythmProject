#pragma once

#include <Math/Vector/Vector2.h>
#include <Features/UI/UISprite.h>
#include <Features/UI/UIGroup.h>

#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/Json/JsonBinder.h>

#include <Features/TextRenderer/TextRenderer.h>

#include <Application/Result/ResultData.h>



#include <cstdint>

class ResultUI
{
public:
    ResultUI() = default;
    ~ResultUI() = default;

    // 初期化
    void Initialize(ResultData _resultData);
    // 更新
    void Update(float _deltaTime);
    // 描画
    void Draw();


private:

    enum class UIs
    {
        MainBackground, // メイン背景

        ScoreBackground, // スコア背景

        ToTitleButton, // タイトルへ戻るボタン
        ReplayButton, // リプレイボタン

    };

    enum class TextType
    {
        Title, // タイトル

        Score_text, // スコア
        Score_value, // スコアの値

        Judge_perfect_text, // パーフェクトのテキスト
        Judge_perfect_value, // パーフェクトの値

        Judge_good_text, // グッドのテキスト
        Judge_good_value, // グッドの値

        Judge_bad_text, // バッドのテキスト
        Judge_bad_value, // バッドの値

        Judge_miss_text, // ミスのテキスト
        Judge_miss_value, // ミスの値

        Combo_text, // コンボのテキスト
        Combo_value, // コンボの値

        Count // テキストの数
    };

private:

    void InitUIGroup();

    void InitTextParams();

    std::string GetKeyString(TextType _textType) const;

    std::wstring GetTextLabel(TextType _textType) const;

    JudgeType GetJudgeTypeFromTextType(TextType _textType) const;

    TextType GetTextTypeFromJudgeType(JudgeType _judgeType) const;

private:

    std::string musicTitle_ = ""; // 楽曲のタイトル

    struct AnimationValue
    {
        Vector2 position = { 0, 0 }; // 座標
        Vector2 movement = { 0, 0 }; // 座標
        Vector2 scale = { 1, 1 }; // スケール
        float alpha = 1.0f; // アルファ値

        float timer = 0.0f;
        float delay = 0.0f; // アニメーションの遅延時間
    };

    struct CounterValue
    {
        int32_t value = 0; // データ保持用
        int32_t currentValue = 0; // 現在の表示値
        float animationTimer = 0.0f; // アニメーションタイマー
    };


    float animationDuration_ = 0.25f;

    std::unique_ptr<UIGroup> uiGroup_ = nullptr; // UIグループ
#ifdef _DEBUG
    std::vector<UISprite*> debugSprites_; // デバッグ用スプライト
    std::vector<UIButton*> debugButtons_; // デバッグ用ボタン
#endif // _DEBUG

    std::unique_ptr<AnimationSequence> animationSequence_ = nullptr; // アニメーションシーケンス

    // textParamの拡張
    struct ExtendedTextParam
    {
        std::wstring label; // ラベル
        TextParam textParam; // テキストパラメータ
        AnimationValue animationValue = {}; // アニメーション用の値
        std::optional<CounterValue> counterValue = std::nullopt; // カウンター用の値（オプション）
    };

    std::map<TextType, ExtendedTextParam> textParams_; // テキストパラメータのマップ

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr; // JSONバインダー
};