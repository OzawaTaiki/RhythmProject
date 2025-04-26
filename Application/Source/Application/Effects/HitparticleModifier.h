#pragma once

#include <Features/Effect/Modifier/ParticleModifier.h>

class HitParticleModifier : public ParticleModifier
{
public:
    HitParticleModifier() = default;
    ~HitParticleModifier() = default;

    void Initialize() override;
    void Apply(Particle* _particle, float _deltaTime) override;
};


class HitCircleParticleModifier : public ParticleModifier
{
public:
    HitCircleParticleModifier() = default;
    ~HitCircleParticleModifier() = default;

    void Initialize() override;
    void Apply(Particle* _particle, float _deltaTime) override;


};