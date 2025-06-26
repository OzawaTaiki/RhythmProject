#include "JudgeResult.h"

#include <Features/Event/EventManager.h>
#include <Debug/ImGuiDebugManager.h>


JudgeResult::JudgeResult()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeResult", [&]() { DebugWindow(); });
#endif // _DEBUG
}

JudgeResult::~JudgeResult()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeResult");
#endif // _DEBUG
}

void JudgeResult::Initialize()
{
    judgeResult_.clear();

    // Noneは除外 1から
    for (size_t i = 1; i < static_cast<size_t>(JudgeType::MAX); ++i)
    {
        JudgeType type = static_cast<JudgeType>(i);
        judgeResult_[type] = 0;
    }

}


void JudgeResult::AddJudge(JudgeType _judgeType)
{
    if (_judgeType == JudgeType::None)
        return; // Noneは除外

    // 判定結果が存在する場合のみカウント
    if (judgeResult_.find(_judgeType) != judgeResult_.end())
    {
        ++judgeResult_[_judgeType]; // 判定結果をカウント
    }
}

void JudgeResult::AddJudge(JudgeType _judgeType, int32_t _count)
{
    if (_judgeType == JudgeType::None || _count <= 0)
        return; // Noneは除外、カウントが0以下の場合は無視

    // 判定結果が存在する場合のみカウント
    if (judgeResult_.find(_judgeType) != judgeResult_.end())
    {
        judgeResult_[_judgeType] += _count; // 判定結果をカウント
    }
}

uint32_t JudgeResult::GetJudgeResult(JudgeType _judgeType) const
{
    auto it = judgeResult_.find(_judgeType);
    if (it != judgeResult_.end())
    {
        return it->second; // 判定結果を返す
    }
    return 0; // 存在しない場合は0を返す
}

void JudgeResult::DebugWindow()
{
#ifdef _DEBUG
    ImGui::PushID(this);
    ImGui::Begin("JudgeResult");
    ImGui::Text("JudgeResult");

    ImGui::Text("Perfect : %d", judgeResult_[JudgeType::Perfect]);
    ImGui::Text("Good : %d", judgeResult_[JudgeType::Good]);
    ImGui::Text("Miss : %d", judgeResult_[JudgeType::Miss]);

    ImGui::Text("None : %d", judgeResult_[JudgeType::None]);

    if (ImGui::Button("Reset"))
    {
        for (auto& [type, count] : judgeResult_)
        {
            count = 0; // 各判定結果をリセット
        }
    }

    ImGui::End();
    ImGui::PopID();

#endif // _DEBUG
}
