#include "GameEnvironment.h"

void GameEnvironment::Initiaize()
{
    GameTime::GetChannel("GameEnvironment");
    // スピーカーの初期化
    speakers_.clear();

    auto speaker = std::make_unique<ObjectModel>("LeftSpeaker");
    speaker->Initialize("Speaker/Speaker.gltf");
    speaker->LoadAnimation("Speaker/Speaker.gltf", "animation");
    speaker->translate_ = Vector3(-8.0f, -3.0, 21.0f); // スピーカーを横に並べる
    speaker->euler_.y = -0.25f + 3.14f;
    speaker->SetTimeChannel("GameEnvironment"); // アニメーションの時間チャンネルを設定
    speakers_.push_back(std::move(speaker));

    speaker = std::make_unique<ObjectModel>("RightSpeaker");
    speaker->Initialize("Speaker/Speaker.gltf");
    speaker->LoadAnimation("Speaker/Speaker.gltf", "animation");
    speaker->translate_ = Vector3(8.0f, -3.0, 21.0f); // スピーカーを横に並べる
    speaker->euler_.y = 0.25f + 3.14f;
    speaker->SetTimeChannel("GameEnvironment"); // アニメーションの時間チャンネルを設定
    speakers_.push_back(std::move(speaker));

    // フロアの初期化
    floors_.clear();

    auto floor = std::make_unique<ObjectModel>("UnderFloor");
    floor->Initialize("cube/cube.obj");
    floor->translate_ = Vector3(0.0f, -4.5f, 18.0f); // フロアの位置を調整
    floor->scale_ = Vector3(15.0f, 0.5f, 15.0f); // フロアのサイズを調整

    floors_.push_back(std::move(floor));

    floor = std::make_unique<ObjectModel>("OverFloor");
    floor->Initialize("cube/cube.obj");
    floor->translate_ = Vector3(0.0f, -3.5f, 30.0f); // フロアの位置を調整
    floor->scale_ = Vector3(15.0f, 0.5f, 15.0f); // フロアのサイズを調整

    floors_.push_back(std::move(floor));

    floorColors_.resize(2);
    floorColors_[0] = { 0.04f, 0.05f, 0.132f, 1.0f }; // UnderFloor
    floorColors_[1] = { 0.78f, 0.52f, 0.23f, 1.0f }; // OverFloor


}

void GameEnvironment::Update(float _deltaTime)
{
    // スピーカーの更新処理
    for (auto& speaker : speakers_)
    {
        if (speaker)
        {
            speaker->Update();
        }
    }

    // フロアの更新処理
    for (auto& floor : floors_)
    {
        if (floor)
        {
            floor->Update();
        }
    }
}

void GameEnvironment::Draw(const Camera* _camera)
{
    for (auto& speaker : speakers_)
    {
        if (speaker)
        {
            speaker->Draw(_camera);
        }
    }


    if(floors_[0])
        floors_[0]->Draw(_camera, floorColors_[0]);
    if (floors_[1])
        floors_[1]->Draw(_camera, floorColors_[1]);

}

void GameEnvironment::SetBPM(float _bpm)
{
    // BPMに基づいて時間スケールを設定

    const float kSpeakerSwingBpmThreshold = 160.0f;// スピーカーのアニメーション速度を変更する閾値

    timeScale_ = 1.0f / (60.0f / _bpm);
    // スピーカーのアニメーション速度を調整 早くなりすぎないようにスケーリング。
    if (_bpm < kSpeakerSwingBpmThreshold)
        timeScale_ *= 0.5f;
    else
        timeScale_ *= 0.25f;

    GameTime::GetChannel("GameEnvironment").SetGameSpeed(timeScale_);

}

void GameEnvironment::StartAnimation()
{
    for (auto& speaker : speakers_)
    {
        if (speaker)
        {
            speaker->SetAnimation("animation", true);
        }
    }
}
