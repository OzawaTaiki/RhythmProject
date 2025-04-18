#pragma once

#include <Features/Event/EventData.h>

#include <vector>

struct NoteJudgeData : public EventData
{
    // ひとつずつではなくまとめて渡したい
    std::vector<int> hmm_;


};