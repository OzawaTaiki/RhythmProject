#include "SettingMenu.h"

#include <System/Input/Input.h>
#include <Debug/Debug.h>
#include <System/Audio/AudioSystem.h>

#include <Application/Setting/Setting.h>

void SettingMenu::Initialize(std::function<void(float)> _speedSetFunc, std::function<void(float)> _audioLatencySetFunc)
{
    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    speedSetFunc_ = _speedSetFunc;
    audioLatencySetFunc_ = _audioLatencySetFunc;

    auto volumeSlider = uiGroup_->CreateSlider("VolumeSlider", 0.0f, 1.0f);
    volumeSlider->SetPos({ 100, 100 });
    volumeSlider->SetSize({ 200, 20 });
    volumeSlider->SetValue(Setting::current_.masterVolume);
    volumeSlider->SetOnValueChanged([](float value) {
        Setting::current_.masterVolume = value; // 音量を設定に反映
        AudioSystem::GetInstance()->SetMasterVolume(value); // オーディオシステムに音量を反映
        Debug::Log(std::format("Volume changed: {}\n", value));
        });

    auto noteSpeedSlider = uiGroup_->CreateSlider("NoteSpeedSlider", 5.0f, 100.0f);
    noteSpeedSlider->SetPos({ 100, 150 });
    noteSpeedSlider->SetSize({ 200, 20 });
    noteSpeedSlider->SetValue(Setting::current_.noteSpeed);
    noteSpeedSlider->SetOnValueChanged([&](float value) {
        Setting::current_.noteSpeed = value; // ノーツ速度を設定に反映
        if (speedSetFunc_)
            speedSetFunc_(value); // コールバック関数を呼び出してノーツ速度を設定
        Debug::Log(std::format("Note speed changed: {}\n", value));
        });

    auto audioLatencySlider = uiGroup_->CreateSlider("AudioLatencySlider", -1000.0f, 1000.0f);
    audioLatencySlider->SetPos({ 100, 200 });
    audioLatencySlider->SetSize({ 200, 20 });
    audioLatencySlider->SetValue(Setting::current_.audioLatencyMs);
    audioLatencySlider->SetOnValueChanged([&](float value) {
        Setting::current_.audioLatencyMs = value; // 音声遅延を設定に反映
        if (audioLatencySetFunc_)
            audioLatencySetFunc_(value); // コールバック関数を呼び出して音声遅延を設定
        Debug::Log(std::format("Audio latency changed: {}\n", value));
        });

    // スライダーをリストに追加
    sliders_.push_back(volumeSlider);
    sliders_.push_back(noteSpeedSlider);
    sliders_.push_back(audioLatencySlider);

    isActive_ = true;
}

void SettingMenu::Update()
{
    if (!isActive_)
        return;

    uiGroup_->Update();
}

void SettingMenu::Draw()
{
    if (!isActive_)
        return;

    uiGroup_->Draw();
}
