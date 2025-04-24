#include "JudgeResult.h"

#include <Features/Event/EventManager.h>
#include <Debug/ImGuiDebugManager.h>

#include <Application/EventData/JudgeResultData.h>

JudgeResult::JudgeResult()
{
    EventManager::GetInstance()->AddEventListener("JudgeResult", this);

#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeResult", [&]() { DebugWindow(); });
#endif // _DEBUG
}

JudgeResult::~JudgeResult()
{
    EventManager::GetInstance()->RemoveEventListener("JudgeResult", this);

#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeResult");
#endif // _DEBUG
}

void JudgeResult::Initialize()
{
    judgeResult_.clear();

    // Noneは除外 1から
    for (size_t i = 1; i < static_cast<size_t>(NoteJudgeType::TypeCount); ++i)
    {
        NoteJudgeType type = static_cast<NoteJudgeType>(i);
        judgeResult_[type] = 0;
    }

}


void JudgeResult::OnEvent(const GameEvent& _event)
{
    if (_event.GetEventType() == "JudgeResult")
    {
        // NOTEJUDGEのデータを取得
        auto data = static_cast<JudgeResultData*>(_event.GetData());
        if (data) // nullptrチェック
        {
            // 判定結果を加算
            judgeResult_[data->judgeType] += 1;
        }
    }
}

void JudgeResult::DebugWindow()
{
#ifdef _DEBUG
    ImGui::PushID(this);
    ImGui::Text("JudgeResult");

    ImGui::Text("Perfect : %d", judgeResult_[NoteJudgeType::Perfect]);
    ImGui::Text("Good : %d", judgeResult_[NoteJudgeType::Good]);
    ImGui::Text("Miss : %d", judgeResult_[NoteJudgeType::Miss]);

    ImGui::Text("None : %d", judgeResult_[NoteJudgeType::None]);

    ImGui::PopID();
#endif // _DEBUG
}
