#pragma once


#include <Features/Effect/Particle/Particle.h>

#include <vector>
#include <string>
#include <cstdint>

class TriggerEffects
{
public:
    TriggerEffects() {};
    ~TriggerEffects() {};

    static  void Emit(const Vector3& _pos);

private:

    static uint32_t countPerEmit_ ; // 一度に発生するパーティクルの数
    static float baseSize ;
    static float centerSize ;

};