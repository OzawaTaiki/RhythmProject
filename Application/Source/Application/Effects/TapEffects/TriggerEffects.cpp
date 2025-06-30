#include "TriggerEffects.h"

#include <Math/Random/RandomGenerator.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Features/Model/Model.h>
#include <Features/Model/Primitive/Plane.h>
#include <Features/Model/Primitive/Triangle.h>

#include <numbers>
#include <Features/Effect/Manager/ParticleSystem.h>

uint32_t TriggerEffects::countPerEmit_ = 16; // 一度に発生するパーティクルの数
float TriggerEffects::baseSize = 0.3f;
float TriggerEffects::centerSize = 1.0f;
uint32_t TriggerEffects::textureHandle_ = 0;
uint32_t TriggerEffects::gradationTexture_ = 0; // グラデーション用のテクスチャ
Vector4 TriggerEffects::commonColor_ = Vector4(0.3f, 0.6f, 1.0f, 0.7f); // 共通の色

ParticleEmitter TriggerEffects::emitter_; // ほそ長いのを出すエミッター
ParticleEmitter TriggerEffects::triangleEmitter_; // 三角形を出すエミッター
ParticleEmitter TriggerEffects::lightPillarEmitter_; // 光の柱を出すエミッター

void TriggerEffects::Initialize()
{
    textureHandle_ = TextureManager::GetInstance()->Load("circle.png");
    gradationTexture_ = TextureManager::GetInstance()->Load("guradation.png");



    // ほそ長いのを出すエミッター
    emitter_.Initialize("TapEffect_01");

    triangleEmitter_.Initialize("TapEffect_Triangle");

    lightPillarEmitter_.Initialize("TapEffect_LightPillar");
}

void TriggerEffects::EmitCenterCircles(const Vector3& _pos)
{
    ParticleRenderSettings settings;
    settings.blendMode = BlendMode::Add;
    settings.cullBack = false;

    {// 中心のやつ
        std::vector<Particle*> circleParticle;
        ParticleInitParam param;

        param.lifeTime = 0.3f;
        param.position = _pos;
        param.position.y += 0.01f;
        param.size = Vector3(centerSize, centerSize * 0.4f, centerSize);
        param.color = commonColor_;
        //param.rotate.x = std::numbers::pi_v<float> / 6.0f;
        param.speed = 0.0f;
        param.isBillboard = { false,false ,true};

        Particle* particle = new Particle();
        particle->Initialize(param);
        circleParticle.push_back(particle);

        param.size = Vector3(centerSize, centerSize * 0.4f, centerSize) * 1.3f;
        param.color = Vector4(0.6f, 0.8f, 1.0f, 0.7f);

        Particle* particle2 = new Particle();
        particle2->Initialize(param);
        circleParticle.push_back(particle2);

        ParticleSystem::GetInstance()->AddParticles("Hit_circle", "pY1x1Plane",
            circleParticle, settings, textureHandle_, { "HitCircleParticleModifier" });
    }
}

void TriggerEffects::EmitSurroundingParticles(const Vector3& _pos)
{
    emitter_.SetPosition(_pos);
    triangleEmitter_.SetPosition(_pos);
    //lightPillarEmitter_.SetPosition(_pos);

    emitter_.GenerateParticles();
    triangleEmitter_.GenerateParticles();
    //lightPillarEmitter_.GenerateParticles();
}
