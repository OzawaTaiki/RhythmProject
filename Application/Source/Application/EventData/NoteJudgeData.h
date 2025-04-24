#pragma once

#include <Features/Event/EventData.h>

#include <Application/Note/Note.h>

#include <vector>
#include <cstdint>

struct NoteJudgeData : public EventData
{
    double diff = 0.0f; // タイミングの差分
    uint32_t laneIndex = 0; // レーンインデックス
    Note* note = nullptr; // 判定を取るノーツのポインタ

    NoteJudgeData(double _diff, uint32_t _laneIndex, Note* _note)
        : diff(_diff), laneIndex(_laneIndex), note(_note) {
    }
    ~NoteJudgeData() override = default;
};