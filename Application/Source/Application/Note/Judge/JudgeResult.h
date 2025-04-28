#pragma once

// Engine
#include  <Features/Event/EventListener.h>

// Application
#include <Application/EventData/NoteJudgeType.h>

// STL
#include <map>
#include <cstdint>

// 判定をもっておくクラス
class JudgeResult : public iEventListener
{
public:
    JudgeResult();
    ~JudgeResult();

    void Initialize();

    void OnEvent(const GameEvent& _event) override;

    void DebugWindow();
private:

    std::map<NoteJudgeType, uint32_t> judgeResult_; // 判定結果



};