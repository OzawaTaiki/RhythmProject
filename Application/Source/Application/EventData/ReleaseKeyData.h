#pragma once

#include <Features/Event/EventData.h>

#include <cstdint>

struct ReleaseKeyData : public EventData
{
    uint32_t laneIndex = 0; // レーンインデックス (vectorなどのはいれつにすべき
    double keyReleasedTimestamp = 0.0; // キーが離された時間
    ReleaseKeyData(uint32_t _laneIndex, double _timestamp)
        : laneIndex(_laneIndex), keyReleasedTimestamp(_timestamp) {
    }

};
