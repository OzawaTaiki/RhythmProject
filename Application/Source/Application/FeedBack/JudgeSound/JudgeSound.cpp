#include "JudgeSound.h"

#include <System/Audio/AudioSystem.h>
#include <Debug/Debug.h>

#include <cassert>

JudgeSound::~JudgeSound()
{
    StopAllSounds(); // 全ての音声を停止

    soundInstance_.reset(); // 音声インスタンスをリセット
}

void JudgeSound::Initialize(float _volume ,const std::string& _soundFilePath)
{
    volume_ = _volume;
    soundFilePath_ = _soundFilePath;

    // 音声インスタンスをロード
    soundInstance_ = AudioSystem::GetInstance()->Load(soundFilePath_);
    if (!soundInstance_)
    {
        Debug::Log("Error: Failed to load sound file: " + soundFilePath_ + "\n");
        assert(false);
    }
}

void JudgeSound::Play()
{
    if (!soundInstance_)
    {
        Debug::Log("Error: Sound instance is not initialized.\n");
        return;
    }

    // 音声インスタンスを生成して再生
    auto voice = soundInstance_->GenerateVoiceInstance(volume_, 0.0f);
    if (voice)
    {
        voice->Play();
        voiceInstance_.push_back(voice); // 再生インスタンスを保存
    }
    else
    {
        Debug::Log("Error: Failed to create voice instance for sound: " + soundFilePath_ + "\n");
        assert(false);
    }
}

void JudgeSound::CleanupStoppedVoices()
{
    if (voiceInstance_.empty())
        return;

    for (auto it = voiceInstance_.begin(); it != voiceInstance_.end();)
    {
        if (*it && !(*it)->IsPlaying())
        {
            it = voiceInstance_.erase(it); // 再生が停止している音声インスタンスを削除
            continue;
        }
        ++it; // 次の要素へ
    }
}

void JudgeSound::StopAllSounds()
{
    if (voiceInstance_.empty())
        return;

    for (auto& voice : voiceInstance_)
    {
        if (voice)
        {
            voice->Stop();
        }
    }
    voiceInstance_.clear();
}
