#pragma once

#include <Features/Event/EventData.h>

#include <cstdint>

struct HitKeyData : public EventData
{
    uint32_t laneIndex = 0; // レーンインデックス
    float keyTriggeredTimestamp = 0.0f; // キーが押された時間

    HitKeyData(uint32_t _laneIndex, float _timestamp)
        : laneIndex(_laneIndex), keyTriggeredTimestamp(_timestamp) {
    }
};