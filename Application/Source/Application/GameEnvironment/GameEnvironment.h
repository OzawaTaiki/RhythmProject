#pragma once

#include <Features/Model/ObjectModel.h>


#include <memory>


class Camera;

// 背景等のオブジェクトを配置する
class GameEnvironment
{

public:

    GameEnvironment() = default;
    ~GameEnvironment() = default;


    void Initiaize();

    void Update(float _deltaTime);

    void Draw(const Camera* _camera);

    void SetBPM(float _bpm);

    void StartAnimation();
private:

    std::vector<std::unique_ptr<ObjectModel>> speakers_ = {};
    std::vector<std::unique_ptr<ObjectModel>> floors_ = {};

    std::vector<Vector4> floorColors_ = {
        { 0.2f, 0.2f, 0.2f, 1.0f }, // UnderFloor
        { 0.3f, 0.3f, 0.3f, 1.0f }  // OverFloor
    };

    float timeScale_ = 1.0f; // アニメーションの時間スケール

};