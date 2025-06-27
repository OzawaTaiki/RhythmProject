#pragma once

#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>


#include <memory>
#include <vector>
#include <string>

// 判定時にサウンドを鳴らすためのクラス
class JudgeSound
{
public:
    // コンストラクタ
    JudgeSound() = default;
    // デストラクタ
    ~JudgeSound();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="_soundFilePath">SEファイルパス</param>
    void Initialize(float _volume = 0.5f , const std::string& _soundFilePath = "Resources/Sounds/SE/JudgeSound.wav");

    /// <summary>
    /// 音声を再生
    /// </summary>
    void Play();

    /// <summary>
    /// 再生されていない音声インスタンスを削除
    /// </summary>
    void CleanupStoppedVoices();

    /// <summary>
    /// 全ての音声を停止
    /// </summary>
    void StopAllSounds();


    /// -----------------------------------------------------
    void SetVolume(float _volume) { volume_ = _volume; }
    float GetVolume() const { return volume_; }

private: // メンバ関数


private: // メンバ変数

    std::shared_ptr<SoundInstance> soundInstance_; // 音声インスタンス
    std::vector<std::shared_ptr<VoiceInstance>> voiceInstance_; // 音声の再生インスタンス

    std::string soundFilePath_; // サウンドファイルのパス


    float volume_ = 0.5f; // 音量

};