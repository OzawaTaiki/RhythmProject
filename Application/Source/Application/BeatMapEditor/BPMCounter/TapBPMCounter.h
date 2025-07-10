#pragma once

#include <System/Input/Input.h>
#include <System/Time/Stopwatch.h>
#include <System/Time/Time_MT.h>

#include <deque>

#include <vector>

class TapBPMCounter
{
public:
    TapBPMCounter() = default;
    ~TapBPMCounter() = default;

    void Initialize();

    void Update();

    void Reset();

    float GetBPM() const;

private:

    Input* input_ = nullptr; // 入力管理クラスへのポインタ

    Stopwatch stopwatch_; // タップの時間を計測するストップウォッチ

    std::deque<float> tapTimes_; // タップした時間を記録するベクター
    float totalTime_ = 0.0f; // タップの合計時間

};