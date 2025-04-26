#include "TriggerEffects.h"

#include <Math/Random/RandomGenerator.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Features/Model/Model.h>

#include <numbers>
#include <Features/Effect/Manager/ParticleSystem.h>

uint32_t TriggerEffects::countPerEmit_ = 16; // 一度に発生するパーティクルの数
float TriggerEffects::baseSize = 0.3f;
float TriggerEffects::centerSize = 1.0f;

void TriggerEffects::Emit(const Vector3& _pos)
{
    auto random = RandomGenerator::GetInstance();
    const Vector4 commonColor = Vector4(0.3f, 0.6f, 1.0f, 0.7f);

    const Vector3 dirMin = Vector3(-0.4f, 1.0f, -0.2f).Normalize();
    const Vector3 dirMax = Vector3(0.4f, 1.0f, 0.2f).Normalize();
    float speedMin = 1.5f; // 初期速度
    float speedMax = 3.0f; // 初期速度

    std::vector<Particle*> particles; // パーティクルのリスト
    std::vector<Particle*> circleParticle;

    {// 中心のやつ
        ParticleInitParam param;

        param.lifeTime = 0.3f;
        param.position = _pos;
        param.size = Vector3(centerSize, centerSize * 0.4f , centerSize);
        param.color = commonColor;
        param.rotate.x = std::numbers::pi_v<float> / 6.0f;
        param.speed = 0.0f;
        param.isBillboard = { true,false ,true };

        Particle* particle = new Particle();
        particle->Initialize(param);
        circleParticle.push_back(particle);

        param.size = Vector3(centerSize, centerSize * 0.4f, centerSize) * 1.2f;

        Particle* particle2 = new Particle();
        particle2->Initialize(param);
        circleParticle.push_back(particle2);
    }

    for (size_t i = 0; i < countPerEmit_ / 2; ++i)
    {// 周囲のパーティクル
        ParticleInitParam param;
        float radius = 0.1f; // 初期半径
        float speed = 3.0f; // 初期速度

        param.lifeTime = random->GetRandValue(0.3f, 0.7f);
        param.position = _pos;
        param.size = Vector3(baseSize * 0.3f, baseSize * 0.7f , baseSize * 0.7f);
        param.color = commonColor;
        param.direction = random->GetRandValue(dirMin, dirMax).Normalize();
        param.speed = random->GetRandValue(speedMin, speedMax);
        param.isBillboard = { false,true ,true }; // 全方向ビルボード

        Particle* particle = new Particle();
        particle->Initialize(param);
        particles.push_back(particle);
    }

    for (size_t i = 0; i < countPerEmit_ / 2; ++i)
    {// 周囲のパーティクル ちっこいの
        ParticleInitParam param;

        float angle = static_cast<float>(i) / (countPerEmit_)*std::numbers::pi_v<float>;
        float radius = 0.1f; // 初期半径


        // サイズ倍率
        const float sizeScale = 0.3f;

        param.lifeTime = random->GetRandValue(0.5f, 0.7f);
        param.position = _pos;
        param.size = Vector3(baseSize, baseSize, baseSize) * sizeScale;
        param.color = commonColor;
        param.direction = random->GetRandValue(dirMin, dirMax).Normalize();
        param.speed = random->GetRandValue(speedMin, speedMax * 0.7f);
        param.isBillboard = { false, true, true }; // 全方向ビルボード

        Particle* particle = new Particle();
        particle->Initialize(param);
        particles.push_back(particle);
    }

    // パーティクルをシステムに追加
    ParticleRenderSettings settings;
    settings.blendMode = BlendMode::Add; // 加算合成
    settings.cullBack = false;

    uint32_t textureHandle = 0;
    textureHandle = TextureManager::GetInstance()->Load("circle.png");

    Model::CreateFromFile("plane/plane.obj");

    ParticleSystem::GetInstance()->AddParticles("Hit_circle", "plane/plane.obj",
        circleParticle, settings, textureHandle, {"HitCircleParticleModifier"});

    ParticleSystem::GetInstance()->AddParticles("Hit_particles", "plane/plane.obj",
        particles, settings, textureHandle, { "HitParticleModifier" });


}
