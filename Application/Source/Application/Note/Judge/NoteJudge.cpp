#include "NoteJudge.h"

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/ImGuiDebugManager.h>

// application
#include <Application/Note/Note.h>

using namespace Engine;
namespace
{
constexpr float kBaseFrameTime = 1.0f / 60.0f; // 60FPS 0.0166s

constexpr std::array<float, 4> timingThresholdFrame = {
    4.0f,  // Perfect: 4フレーム 約0.066s
    10.0f, // Good: 10フレーム 約0.166s
    16.0f, // Bad: 16フレーム 約0.266s
    20.0f  // Miss: 20フレーム 約0.333s
};
}

NoteJudge::NoteJudge()
{
#ifdef _DEBUG
    // デバッグウィンドウの登録
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeLine", [this]() { ImGui::Checkbox("DrawLine", &isDrawLine); });
#endif // _DEBUG
}

NoteJudge::~NoteJudge()
{
#ifdef _DEBUG
    // デバッグウィンドウの削除
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeLine");
#endif // _DEBUG
}

void NoteJudge::Initialize()
{
    InitializeJsonBinder();

    for (size_t i = 0; i < timingThresholdFrame.size(); ++i)
    {
        JudgeType judgeType = static_cast<JudgeType>(i);
        timingThresholds_[judgeType] = kBaseFrameTime * timingThresholdFrame[i];
    }
}

void NoteJudge::DrawJudgeLine()
{
#ifdef _DEBUG
    // 判定ラインと判定範囲の描画

    if (!isDrawLine) return;



    float halfWidth = laneTotalWidth_ / 2.0f;

    // 判定ラインを描画
    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 判定ラインを描画
        float position = timingThresholds_[i] * speed_ + position_;

        Vector3 start = { -halfWidth, 0.01f,  position };
        Vector3 end = { halfWidth, 0.01f, position };

        LineDrawer::GetInstance()->RegisterPoint(start, end, Vector4(1, 1, 0, 1));
    }


    // ライン手前の判定線を描画
    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 判定ラインを描画
        float position = (-timingThresholds_[i] * speed_) + position_;

        Vector3 start = { -halfWidth, 0,  position };
        Vector3 end = { halfWidth, 0, position };

        LineDrawer::GetInstance()->RegisterPoint(start, end, Vector4(1, 1, 0, 1));
    }
#endif // DEBUG

}

JudgeType NoteJudge::ProcessNoteJudge(Note* note, float elapsedTime)
{
    if (note == nullptr)
        return JudgeType::None; // nullチェック

    JudgeType result = JudgeType::None;

    float targetTime = note->GetTargetTime();

    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 判定範囲内かチェック
        if (targetTime >= elapsedTime - timingThresholds_[i] &&
            targetTime <= elapsedTime + timingThresholds_[i])
        {
            // 蛻､螳壹ｒ陦後≧
            result = static_cast<JudgeType>(i);
            break;
        }
    }

    return result;
}


void NoteJudge::InitializeJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("Judge", "Resources/Data/Note/");

    std::vector<float> timingThresholds;
    jsonBinder_->GetVariableValue("TimingThresholds", timingThresholds_);


    for (size_t i = 0; i < timingThresholds.size(); ++i)
    {
        JudgeType judgeType = static_cast<JudgeType>(i);
        timingThresholds_[judgeType] = timingThresholds[i];
    }

}