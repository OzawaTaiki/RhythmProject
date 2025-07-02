#include "JudgeText.h"

#include <Features/TextRenderer/TextRenderer.h>
#include <Math/Mylib.h>

#include <Application/Lane/Lane.h>

float JudgeText::displayYOffset_ = -100.0f; // Y軸のオフセットを初期化

JudgeText::JudgeText():
    judgeType_(JudgeType::None),
    position_(0.0f, 0.0f),
    timer_(0.0f),
    displayDuration_(1.0f), // 初期表示時間を設定
    textRenderer_(TextRenderer::GetInstance()) // テキストレンダラーのインスタンスを取得
{
}

void JudgeText::Initialize(JudgeType _judgeType, int32_t _laneIndex, const Camera* _camera)
{
    position_ = _camera->WotldToScreen(Lane::GetLaneEndPosition(_laneIndex));
    position_.y += displayYOffset_; // Y軸のオフセットを適用

    judgeType_ = _judgeType; // 判定タイプを設定

    timer_ = 0.0f; // タイマー初期化
    displayDuration_ = 1.0f; // 表示時間の初期化

    judgeText_ = GetJudgeText(judgeType_); // 判定テキストの初期化
    GetJudgeTextColor(judgeType_, topColor_, bottomColor_); // 判定テキストの色を取得

    if(!textRenderer_)
        textRenderer_ = TextRenderer::GetInstance(); // テキストレンダラーのインスタンスを取得

}

void JudgeText::Update(float _deltaTime)
{
    timer_ += _deltaTime;

    // 表示時間を超えたら終了
    if (timer_ >= displayDuration_)
    {
        judgeType_ = JudgeType::None; // 判定をリセット
        judgeText_.clear(); // テキストをクリア
    }
}

void JudgeText::Draw()
{
    textRenderer_->DrawText(judgeText_, position_, topColor_, bottomColor_);
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
