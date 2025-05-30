#include "ParticleModifierFactory.h"

#include <Features/Effect/Modifier/Preset/RotationBasedMovementModifier.h>
#include <Features/Effect/Modifier/Preset/AlphaOverLifetime.h>
#include <Features/Effect/Modifier/Preset/DecelerationModifier.h>

#include <Application/Effects/TapEffects/HitparticleModifier.h>
#include <Application/Effects/TapEffects/LightPillarModifier.h>


std::unique_ptr<ParticleModifier> ParticleModifierFactory::CreateModifier(const std::string _name)
{
    if (_name == "DecelerationModifier")
    {
        return std::make_unique<DecelerationModifier>();
    }
    else if (_name == "AlphaOverLifetime")
    {
        return std::make_unique<AlphaOverLifetime>();
    }
    else if (_name == "ParticleModifier2")
    {
        // return std::make_unique<ParticleModifier2>();
    }
    else if (_name == "HitParticleModifier")
    {
        return std::make_unique<HitParticleModifier>();
    }
    else if (_name == "HitCircleParticleModifier")
    {
        return std::make_unique<HitCircleParticleModifier>();
    }
    else if (_name == "RotationBasedMovementModifier")
    {
         return std::make_unique<RotationBasedMovementModifier>();
    }
    else if (_name == "LightPillarModifier")
    {
        return std::make_unique<LightPillarModifier>();
    }


    throw std::runtime_error("モディファイアが見つかりませんでした。");

    return nullptr;
}
