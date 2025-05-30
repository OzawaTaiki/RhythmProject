#pragma once


#include <Features/Effect/Particle/Particle.h>
#include <Features/Effect/Emitter/ParticleEmitter.h>

#include <vector>
#include <string>
#include <cstdint>

class TriggerEffects
{
public:
    TriggerEffects() {};
    ~TriggerEffects() {};

    static void Initialize();

    // 中心の円を発生させる
    static void EmitCenterCircles(const Vector3& _pos);
    // 周囲のパーティクルを発生させる
    static void EmitSurroundingParticles(const Vector3& _pos);


private:

    static uint32_t countPerEmit_ ; // 一度に発生するパーティクルの数
    static float baseSize ;
    static float centerSize ;

    static Vector4 commonColor_;

    static uint32_t textureHandle_;
    static uint32_t gradationTexture_;


    static ParticleEmitter emitter_; // ほぞ長いのを出すエミッター
    static ParticleEmitter triangleEmitter_; // 三角形を出すエミッター

};