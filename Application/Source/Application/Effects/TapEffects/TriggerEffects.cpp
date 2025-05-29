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
Vector4 TriggerEffects::commonColor_ = Vector4(0.3f, 0.6f, 1.0f, 0.7f); // 共通の色

ParticleEmitter TriggerEffects::emitter_; // ほそ長いのを出すエミッター
ParticleEmitter TriggerEffects::triangleEmitter_; // 三角形を出すエミッター

void TriggerEffects::Initialize()
{
    textureHandle_ = TextureManager::GetInstance()->Load("circle.png");

    // 2x2 y+向き
    Plane plane;
    plane.SetSize(Vector2(1.0f, 1.0f) * 2);
    plane.SetNormal(Vector3(0, 1, 0));
    plane.SetPivot(Vector3(0, 0, 0));

    plane.Generate("pY1x1Plane");

    //// 2x2 z-向き
    //Plane nZplane;
    //nZplane.SetSize(Vector2(1.0f, 1.0f) * 2);
    //nZplane.SetNormal(Vector3(0, 0, -1));
    //nZplane.SetPivot(Vector3(0, 0, 0));

    //nZplane.Generate("nZ1x1Plane");

    // ほそ長いやつ
    Plane plane2;
    plane2.SetSize(Vector2(0.1f, 0.7f) * 5.0f);
    plane2.SetNormal(Vector3(0, 0, -1));
    plane2.SetPivot(Vector3(0, 0, 0));

    plane2.Generate("nZ0.1x0.7Plane");


    Triangle triangle;
    triangle.SetNormal(Vector3(0, 0, -1));
    triangle.SetVertices({
        Vector3(0, 0.5f, 0),
        Vector3(0.5f, -0.5f, 0),
        Vector3(-0.5f, -0.5f, 0)
        });
    triangle.Generate("nZ1_1Triangle");

    // ほそ長いのを出すエミッター
    emitter_.Initialize("TapEffect_01");

    triangleEmitter_.Initialize("TapEffect_Triangle");
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

    emitter_.GenerateParticles();
    triangleEmitter_.GenerateParticles();

}
