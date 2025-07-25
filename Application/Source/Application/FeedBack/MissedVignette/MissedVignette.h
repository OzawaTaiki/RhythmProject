#pragma once

#include <Features/PostEffects/Vignette.h>

#include <memory>


class MissedVignette
{
public:
    MissedVignette() = default;
    ~MissedVignette() = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update(float _deltaTime);

    /// <summary>
    /// エフェクトを適用する
    /// </summary>
    void ApplyEffect(const std::string& _input, const std::string& _output);


    void Emit();
private:

    bool emit_ = false;

    float duration_ = 0.5f; // エフェクトの持続時間
    float elapsedTime_ = 0.0f; // 経過時間

    std::unique_ptr<Vignette> vignette_ = nullptr;
    VignetteData currentVignetteData_ = {};

    VignetteData startVignetteData_ = {};

};