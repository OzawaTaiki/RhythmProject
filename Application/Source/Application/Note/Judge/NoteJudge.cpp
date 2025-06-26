#include "NoteJudge.h"

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/ImGuiDebugManager.h>

// application
#include <Application/Note/Note.h>


// STL

NoteJudge::NoteJudge()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeLine", [this]() { ImGui::Checkbox("DrawLine", &isDrawLine); });
#endif // _DEBUG
}

NoteJudge::~NoteJudge()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeLine");
#endif // _DEBUG
}

void NoteJudge::Initialize()
{
    InitializeJsonBinder();

    // 仮
    timingThresholds_[JudgeType::Perfect] = 0.06f;
    timingThresholds_[JudgeType::Good] = 0.18f;
    timingThresholds_[JudgeType::Miss] = 0.3f;

}

void NoteJudge::DrawJudgeLine()
{
    if (!isDrawLine) return;

    /// debug用


    float halfWidth = laneTotalWidth_ / 2.0f;

    // 判定ラインを描画
    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 判定ラインを描画
        float position = timingThresholds_[i] * speed_ + position_;

        Vector3 start = { -halfWidth, 0,  position };
        Vector3 end = { halfWidth, 0, position };

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

}

JudgeType NoteJudge::ProcessNoteJudge(Note* _note, float _elapsedTime)
{
    if (_note == nullptr)
        return JudgeType::None; // nullチェック

    JudgeType result = JudgeType::None;

    float targetTime = _note->GetTargetTime();

    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        if (targetTime >= _elapsedTime - timingThresholds_[i] &&
            targetTime <= _elapsedTime + timingThresholds_[i])
        {
            // 判定を行う
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
