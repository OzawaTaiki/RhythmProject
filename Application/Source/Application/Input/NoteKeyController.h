#pragma once

// Engine
#include <System/Time/Stopwatch.h>
#include <System/Input/Input.h>
#include <Features/Json/JsonBinder.h>
#include <Features/Event/EventListener.h>

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/Judge/NoteJudge.h>

// STL
#include <vector>
#include <cstdint>

class NoteKeyController : public iEventListener
{
public:
    NoteKeyController();
    ~NoteKeyController();

    void Initialize(Stopwatch* _stopWatch, NoteJudge* _noteJudge);
    void Update();
    void Draw();

    void OnEvent(const GameEvent& _event) override;

private:

    void InitializeJsonBinder();

private:

    Stopwatch* stopWatch_;          // 時間管理用（所有権なし）
    NoteJudge* noteJudge_;          // 判定用（所有権なし）
    Input* input_;

    
    bool HoldKey = false; // キーを押しているかどうか
    

    // キー設定
    std::vector<uint32_t> laneKeyBindings_;

    std::unique_ptr<JsonBinder> jsonBinder_;

};