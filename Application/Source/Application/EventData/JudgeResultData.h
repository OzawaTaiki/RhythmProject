#pragma once

#include <Features/Event/EventData.h>

#include <Application/EventData/NoteJudgeType.h>

#include <cstdint>

struct JudgeResultData : public EventData
{
    NoteJudgeType judgeType = NoteJudgeType::None; // 判定の種類

    uint32_t laneIndex = 0; // レーンインデックス

    JudgeResultData(NoteJudgeType _judgeType, uint32_t _laneIndex)
        : judgeType(_judgeType), laneIndex(_laneIndex) {
    }
    ~JudgeResultData() override = default;
};