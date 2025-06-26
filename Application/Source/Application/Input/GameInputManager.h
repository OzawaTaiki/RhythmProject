#pragma once
#include <System/Audio/VoiceInstance.h>
#include <System/Input/Input.h>

#include <map>
#include <cstdint>
#include <memory>
#include <stdexcept>


enum class KeyState
{
    None,      // 何も押されていない
    trigger,   // 押された
    Released,  // 離された
    Hold,       // 押し続けている

    MAX // キー状態の最大値
};

struct InputDate
{
    KeyState state = KeyState::None; // キーの状態
    float elapsedTime = 0.0f; // 経過時間
};

class GameInputManager
{
public:
    /// コンストラクタ
    GameInputManager() = default;
    /// デストラクタ
    ~GameInputManager() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="_input"> Inputのインスタンス </param>
    void Initialize(Input* _input);


    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 入力データを取得する
    /// </summary>
    /// <returns>入力データのベクター </returns>
    const std::vector<InputDate>& GetInputData() const { return inputData_; }


    /// <summary>
    /// キーバインドの設定
    /// </summary>
    /// <param name="_keyBindings">キーとレーンのバインディングのマップ</param>
    void SetKeyBinding(std::map<int8_t, int32_t> _keyBindings) { keyBindings_ = _keyBindings; }

    /// <summary>
    /// キーとレーンのバインディングを設定
    /// </summary>
    /// <param name="_key"> キーコード (DIK_Aなど)</param>
    /// <param name="_lane"> レーン番号</param>
    void SetKeyBinding(int8_t _key, int32_t _lane) { keyBindings_[_key] = _lane; }

    /// <summary>
    /// 音楽の音声インスタンスを設定
    /// </summary>
    /// <param name="_voiceInstance">音声インスタンスのポインタ </param>
    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> _voiceInstance) { musicVoiceInstance_ = _voiceInstance; }
private:

    /// <summary>
    /// デフォルトのキー設定を行う
    /// </summary>
    void SetDefaultKeyBindings();

private:
    Input* input_; // Inputのインスタンス

    std::vector<InputDate> inputData_; // キーの状態を保持するベクター

    std::map<int8_t, int32_t> keyBindings_; // キーのバインディング

    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス
};