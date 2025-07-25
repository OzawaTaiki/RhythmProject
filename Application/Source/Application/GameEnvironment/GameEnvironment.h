#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/UVTransform/SpriteSheetAnimetion.h>


#include <memory>


class Camera;

// 背景等のオブジェクトを配置する
class GameEnvironment
{

public:

    GameEnvironment() = default;
    ~GameEnvironment() = default;


    void Initiaize(const std::string& _filePath = "Resources/Data/Game/Environment.json");

    void Update(float _deltaTime);

    void Draw(const Camera* _camera);

    void SetBPM(float _bpm);

    void StartAnimation();
private:

    void Serialize(const std::string& _filePath);


private:
    std::vector<std::unique_ptr<ObjectModel>> environmentObjects_ = {};
    std::unique_ptr<ObjectModel> overFloor_ = nullptr;

    SpriteSheetAnimation spriteSheetAnimation_; // 連番画像アニメーション

    std::vector<Vector4> floorColors_ = {
        { 0.2f, 0.2f, 0.2f, 1.0f }, // UnderFloor
        { 0.3f, 0.3f, 0.3f, 1.0f }  // OverFloor
    };

    float timeScale_ = 1.0f; // アニメーションの時間スケール

};