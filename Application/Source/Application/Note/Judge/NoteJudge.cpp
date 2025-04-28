#include "NoteJudge.h"

// Engine
#include <Features/Event/EventManager.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/ImGuiDebugManager.h>

// application
#include <Application/EventData/NoteJudgeData.h>
#include <Application/EventData/JudgeResultData.h>


// STL

NoteJudge::NoteJudge()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeLine", [this]() { ImGui::Checkbox("DrawLine", &isDrawLine); });
#endif // _DEBUG


    EventManager::GetInstance()->AddEventListener("NoteJudge", this);
}

NoteJudge::~NoteJudge()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeLine");
#endif // _DEBUG

    EventManager::GetInstance()->RemoveEventListener("NoteJudge", this);
}

void NoteJudge::Initialize()
{
    InitializeJsonBinder();

    // 仮
    timingThresholds_[NoteJudgeType::Perfect] = 0.06f;
    timingThresholds_[NoteJudgeType::Good] = 0.18f;
    timingThresholds_[NoteJudgeType::Miss] = 0.3f;


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


void NoteJudge::OnEvent(const GameEvent& _event)
{
    if (_event.GetEventType() == "NoteJudge")
    {
        // NOTEJUDGEのデータを取得
        auto data = static_cast<NoteJudgeData*>(_event.GetData());
        if (data)// nullチェック
        {
            // 判定を取得
            for (const auto& [i, timingThreshold] : timingThresholds_)
            {
                if (data->diff >= 0) // 奥にある
                {
                    if (data->diff <= timingThresholds_[i])
                    {
                        // 判定を行う
                        JudgeResultData judgeResult(i, data->laneIndex);
                        EventManager::GetInstance()->DispatchEvent(
                            GameEvent("JudgeResult", &judgeResult)
                        );
                        break;
                    }
                }
                else
                {
                    if (data->diff >= -timingThresholds_[i])
                    {
                        // 判定を行う
                        JudgeResultData judgeResult(i, data->laneIndex);
                        EventManager::GetInstance()->DispatchEvent(
                            GameEvent("JudgeResult", &judgeResult)
                        );
                        break;
                    }
                }
            }

            // ミス判定

        }
    }
}

void NoteJudge::InitializeJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("Judge", "Resources/Data/Note/");

    std::vector<float> timingThresholds;
    jsonBinder_->GetVariableValue("TimingThresholds", timingThresholds_);


    for (size_t i = 0; i < timingThresholds.size(); ++i)
    {
        NoteJudgeType judgeType = static_cast<NoteJudgeType>(i);
        timingThresholds_[judgeType] = timingThresholds[i];
    }

}
