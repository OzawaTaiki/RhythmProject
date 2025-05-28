#pragma once

#include <Math/Vector/Vector3.h>

#include <Features/Model/ObjectModel.h>
#include <Features/Model/Primitive/Plane.h>

#include <vector>

class LaneEffect
{
public:

    LaneEffect() = default;
    ~LaneEffect() = default;

    void Initialize(const Vector3& _laneCenter,Model* _planeModel);

    void Update(float _deltaTime);

    void Draw(const Camera* _camera);

    void Start();

    void End();

    void SetDuration(float _duration) { duration_ = _duration; }
    float GetDuration() const { return duration_; }

    void SetColor(const Vector4& _color) { color_ = _color; }
    float GetTimer() const { return timer_; }

private:

    float duration_ = 0.0f; // エフェクトの持続時間
    float timer_ = 0.0f; // エフェクトのタイマー

    bool isActive_ = true; // エフェクトがアクティブかどうか

    Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f }; // エフェクトの色

    std::unique_ptr<ObjectModel> laneModel_;

};