#pragma once

#include <Features/Scene/Interface/ISceneTransition.h>

#include <Features/Sprite/Sprite.h>

class SceneTrans : public ISceneTransition
{
public:
    SceneTrans() = default;
    ~SceneTrans() override = default;
    // ISceneTransitionの実装

    void Initialize() override;
    void Update() override;
    void Draw() override;

    bool IsEnd() override { return isEnd_; };
    bool CanSwitch() override { return canSwitch_; };

    void Start() override;
    void End() override;

private:

    float duration_ = 1.0f; // トランジションの持続時間
    float elapsedTime_ = 0.0f; // 経過時間
    float alpha_ = 0.0f; // トランジションのアルファ値
    float fadeSpeed_ = 1.0f; // フェード速度

    std::unique_ptr<Sprite> transitionSprite_ = nullptr; // トランジション用のスプライト

    bool isEnd_ = false; // トランジションが終了したか
    bool canSwitch_ = false; // シーンを切り替え可能か
    bool playing_ = false; // トランジションが再生中か

};