#include "SpeakerEffect.h"

#include <Features/Model/ObjectModel.h>

SpeakerEffect::SpeakerEffect()
{

    ringEmitter_.Initialize("SpeakerRing");
    rectanglleParticleEmitter_.Initialize("Speaker_particle");
    triangleParticleEmitter_.Initialize("Sepaker_Triangle");
}

void SpeakerEffect::PlaySpeakerEffect(const Vector3& _pos)
{

    ringEmitter_.SetPosition(_pos);
    triangleParticleEmitter_.SetPosition(_pos);
    rectanglleParticleEmitter_.SetPosition(_pos);

    ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectanglleParticleEmitter_.GenerateParticles();
}

void SpeakerEffect::PlaySpeakerEffect(ObjectModel* _parent)
{

    ringEmitter_.SetParentTransform(_parent->GetWorldTransform());
    triangleParticleEmitter_.SetParentTransform(_parent->GetWorldTransform());
    rectanglleParticleEmitter_.SetParentTransform(_parent->GetWorldTransform());

    //ringEmitter_.GenerateParticles();
    triangleParticleEmitter_.GenerateParticles();
    rectanglleParticleEmitter_.GenerateParticles();
}
