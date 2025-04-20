#pragma once

#include <Features/Event/EventData.h>

#include <cstdint>

struct HitKeyData : public EventData
{
    uint32_t laneIndex = 0; // レーンインデックス
    double keyTriggeredTimestamp = 0.0; // キーが押された時間

    HitKeyData(uint32_t _laneIndex, double _timestamp)
        : laneIndex(_laneIndex), keyTriggeredTimestamp(_timestamp) {
    }
};