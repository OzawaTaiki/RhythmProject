#include "BeatManager.h"
#include <System/Audio/Audio.h>
#include <System/Time/GameTime.h>
#include <Debug/Debug.h>
#include <cmath>

BeatManager::BeatManager() : 
    bpm_(120.0f), 
    offset_(0.0f), 
    lastBeat_(0), 
    playing_(false),
    soundHandle_(0),
    voiceHandle_(0),
    volume_(0.5f),
    soundEnabled_(true)
{
}

void BeatManager::Initialize(float bpm, float offset, const std::string& soundPath)
{
    bpm_ = bpm;
    offset_ = offset;
    lastBeat_ = -1;
    playing_ = false;
    
    // サウンドを読み込む
    if (soundEnabled_)
    {
        soundHandle_ = Audio::GetInstance()->SoundLoadWave(soundPath);
    }
}

void BeatManager::Update()
{
    if (!playing_) return;
    
    // ストップウォッチを更新
    //stopwatch_->Update();
    
    // 新しい拍かチェック
    if (IsNewBeat() && soundEnabled_)
    {
        // 拍に合わせて音を鳴らす
        Debug::Log("Beat Triggered: " + std::to_string(GetNearestBeat()) + "\n");
        //Debug::Log("Elapsed Time: " + std::to_string(stopwatch_->GetElapsedTime<float>()) + "\n");

        voiceHandle_ = Audio::GetInstance()->SoundPlay(soundHandle_, volume_, false, true);
    }
}

void BeatManager::Start()
{
    if (!playing_)
    {
        playing_ = true;
        //Debug::Log("Elapsed Time: " + std::to_string(stopwatch_->GetElapsedTime<float>()) + "\n");
        //stopwatch_->Start();
    }
}

void BeatManager::Stop()
{
    if (playing_)
    {
        playing_ = false;
        //stopwatch_->Stop();
        
        // 音を停止
        if (soundEnabled_)
        {
            Audio::GetInstance()->SoundStop(voiceHandle_);
        }
    }
}

void BeatManager::Reset()
{
    //stopwatch_->Reset();
    lastBeat_ = 0;
    
    // 音を停止
    if (soundEnabled_)
    {
        Audio::GetInstance()->SoundStop(voiceHandle_);
    }
}

float BeatManager::GetCurrentBeat() const
{
    float currentTime = stopwatch_->GetElapsedTime<float>() - offset_;
    return currentTime / GetSecondsPerBeat();
}

int BeatManager::GetNearestBeat() const
{
    return static_cast<int>(std::round(GetCurrentBeat()));
}

bool BeatManager::IsNewBeat()
{
    int currentBeat = GetNearestBeat();
    if (currentBeat > lastBeat_)
    {
        lastBeat_ = currentBeat;
        return true;
    }
    return false;
}

bool BeatManager::IsBeatTriggered(float tolerance) const
{
    float currentBeat = GetCurrentBeat();
    float fractionalPart = currentBeat - std::floor(currentBeat);
    
    // 拍のタイミング内かをチェック
    return (fractionalPart < tolerance || fractionalPart > (1.0f - tolerance));
}

void BeatManager::SetBPM(float bpm)
{
    if (bpm <= 0.0f) return;
    bpm_ = bpm;
    Reset();
}