#include "HitparticleModifier.h"

void HitParticleModifier::Initialize()
{
}

void HitParticleModifier::Apply(Particle* _particle, float _deltaTime)
{
    float t = _particle->GetCurrentTime() / _particle->GetLifeTime();

    float easedt = Easing::Func(Easing::EasingFunc::EaseInOutCirc)(t);
    Vector4 color = _particle->GetColor();
    color.w = 1.0f - easedt;

    _particle->SetColor(color);

    // 減速
    float deceleration = 3.0f;
    float speed = _particle->GetSpeed();
    speed -= deceleration * _deltaTime;
    if (speed < 0.0f)
        speed = 0.0f;
    _particle->SetSpeed(speed);
}

void HitCircleParticleModifier::Initialize()
{
}

void HitCircleParticleModifier::Apply(Particle* _particle, float _deltaTime)
{
    float t = _particle->GetCurrentTime() / _particle->GetLifeTime();

    float easedt = Easing::Func(Easing::EasingFunc::EaseInCubic)(t);

    Vector4 color = _particle->GetColor();
    color.w = 1.0f - easedt;

    _particle->SetColor(color);


    float easedTSize = Easing::Func(Easing::EasingFunc::EaseInOutCirc)(t);

    Vector3 size = _particle->GetScale();
    const float minSize = 0.3f;

    size.x *= easedTSize;
    if (size.x < minSize)
        size.x = minSize;

}