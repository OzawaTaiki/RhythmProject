#include "JudgeEffect.h"

#include <Application/Lane/Lane.h>
#include <Application/Effects/TapEffects/TriggerEffects.h>

void JudgeEffect::Initialize()
{
    TriggerEffects::Initialize();
}

void JudgeEffect::Play(int32_t _laneIndex)
{
    Vector3  lanePos = Lane::GetLaneEndPosition(_laneIndex); // レーンの開始位置を取得

    // 中心の円を発生させる
    TriggerEffects::EmitCenterCircles(lanePos);

    // 周囲のパーティクルを発生させる
    TriggerEffects::EmitSurroundingParticles(lanePos);

    // 光の柱を発生させる
    //TriggerEffects::EmitLightPillar(Vector3(0, 0, 0), Vector3(0, 1, 0), 1.0f);
}
