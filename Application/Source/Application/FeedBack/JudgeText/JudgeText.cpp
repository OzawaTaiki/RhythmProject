#include "JudgeText.h"

#include <Math/Mylib.h>

#include <Application/Lane/Lane.h>

float JudgeText::displayYOffset_ = -150.0f; // Y軸のオフセットを初期化

JudgeText::JudgeText():
    judgeType_(JudgeType::None),
    position_(0.0f, 0.0f),
    timer_(0.0f),
    movement_(0.0f, 0.0f), // 初期位置を設定
    alpha_(1.0f), // 初期アルファ値を設定
    scale_(1.0f, 1.0f), // 初期スケールを設定
    displayDuration_(1.0f), // 初期表示時間を設定
    text_({}) // テキストレンダラーのインスタンスを取得
{
}

void JudgeText::Initialize(JudgeType _judgeType, int32_t _laneIndex, const Camera* _camera)
{
    position_ = _camera->WotldToScreen(Lane::GetLaneEndPosition(_laneIndex));
    position_.y += displayYOffset_; // Y軸のオフセットを適用

    judgeType_ = _judgeType; // 判定タイプを設定

    timer_ = 0.0f; // タイマー初期化
    displayDuration_ = 1.0f; // 表示時間の初期化

    movement_ = Vector2(0.0f, 0.0f); // テキストの移動量を初期化
    alpha_ = 1.0f; // テキストのアルファ値を初期化
    scale_ = Vector2(1.0f, 1.0f); // テキストのスケールを初期化


    judgeText_ = GetJudgeText(judgeType_); // 判定テキストの初期化
    GetJudgeTextColor(judgeType_, topColor_, bottomColor_); // 判定テキストの色を取得

    text_.Initialize(FontConfig()); // テキストレンダラーの初期化

    animationSequence_ = std::make_unique<AnimationSequence>("JudgeTextAnimation");
    animationSequence_->Initialize("Resources/Data/AnimSeq/"); // アニメーションシーケンスの初期化

}

void JudgeText::Update(float _deltaTime)
{
    timer_ += _deltaTime;

    AnimateText();
    UpdateTextParam();

    // 表示時間を超えたら終了
    if (timer_ >= displayDuration_)
    {
        judgeType_ = JudgeType::None; // 判定をリセット
        judgeText_.clear(); // テキストをクリア
    }

}

void JudgeText::Draw()
{
    text_.Draw(judgeText_, textParam_);
}

void JudgeText::AnimateText()
{
    animationSequence_->Update(0.016f);

    alpha_ = animationSequence_->GetValue<float>("color_alpha"); // アルファ値の取得
    scale_ = animationSequence_->GetValue<Vector2>("scale"); // テキストのスケールの取得
    movement_ = animationSequence_->GetValue<Vector2>("movement"); // テキストの移動量の取得

    topColor_.w = alpha_; // 上頂点の色にアルファ値を適用
    bottomColor_.w = alpha_; // 下頂点の色にアルファ値を適用

}

std::wstring JudgeText::GetJudgeText(JudgeType _judgeType)
{
	switch (_judgeType)
	{
	case JudgeType::Perfect:
        return L"PERFECT";
		break;

	case JudgeType::Good:
        return L"GOOD";
		break;

	case JudgeType::Miss:
        return L"MISS";
		break;

	case JudgeType::None:
	case JudgeType::MAX:
	default:
        // 判定なしの場合はerrorを返す
        return L"error";
		break;
	}

    return L"error";
}

void JudgeText::GetJudgeTextColor(JudgeType _judgeType, Vector4& _topColor, Vector4& _bottomColor)
{
    switch (_judgeType)
    {
    case JudgeType::Perfect:
        // ピンクから水色
        _topColor = ColorCodeToVector4(0xff66b8ff); // ピンク色
        _bottomColor = Vector4(0.0f, 1.0f, 1.0f, 1.0f); // 水色
        break;
    case JudgeType::Good:
        // 緑から薄い緑
        _topColor = Vector4(0.0f, 1.0f, 0.0f, 1.0f); // 緑色
        _bottomColor = Vector4(0.5f, 1.0f, 0.5f, 1.0f); // 薄い緑色
        break;
    case JudgeType::Miss:
        // グレー
        _topColor = Vector4(0.5f, 0.5f, 0.5f, 1.0f); // グレー
        _bottomColor = Vector4(0.5f, 0.5f, 0.5f, 1.0f); // グレー
        break;
    case JudgeType::None:
    case JudgeType::MAX:
    default:
        _topColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 白色
        _bottomColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        break;
    }
}

void JudgeText::UpdateTextParam()
{
    textParam_
        .SetPosition(position_ + movement_)
        .SetScale(scale_)
        .SetRotate(0.0f) // 回転はなし
        .SetPivot({ 0.5f, 0.5f }) // ピボットは中央
        .SetGradientColor(topColor_, bottomColor_) // グラデーションカラーを設定
        //.SetOutline({ 1,1,1,1 }, 0.03f)
        ;
}
