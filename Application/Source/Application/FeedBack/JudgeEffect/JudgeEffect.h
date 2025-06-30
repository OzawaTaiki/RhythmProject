#pragma once

#include <cstdint>

class JudgeEffect
{
public:
    JudgeEffect() = default;
    ~JudgeEffect() = default;

    void Initialize();

    /// <summary>
    /// エフェクトを再生
    /// </summary>
    void Play(int32_t _laneIndex);

private:


};