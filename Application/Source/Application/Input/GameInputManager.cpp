#include "GameInputManager.h"

#include <Debug/Debug.h>

void GameInputManager::Initialize(Input* _input)
{
    if (_input == nullptr)
        input_ = Input::GetInstance();
    else
        input_ = _input;

    SetDefaultKeyBindings();

    inputData_.clear();
}

void GameInputManager::Update()
{
    if (auto musicVoiceInstance = musicVoiceInstance_.lock())
    {
        inputData_.clear(); // 前回の入力データをクリア

        for (const auto& [keycode, laneIndex] : keyBindings_)
        {
            InputDate inputData;
            inputData.elapsedTime = musicVoiceInstance->GetElapsedTime();
            inputData.laneIndex = laneIndex;

            if (input_->IsKeyTriggered(keycode))
            {
                inputData.state = KeyState::trigger;

                inputData_.push_back(inputData);

                Debug::Log("Key triggered: " + std::to_string(keycode) + " on lane " + std::to_string(laneIndex) + "\n");
            }
            else if (input_->IsKeyPressed(keycode))
            {
                inputData.state = KeyState::Hold;
                inputData_.push_back(inputData);
            }
            else if (input_->IsKeyReleased(keycode))
            {
                inputData.state = KeyState::Released;
                inputData_.push_back(inputData);
            }
        }
    }
    else
    {
        Debug::Log("Music voice instance is not set.\n");
    }
}

void GameInputManager::SetDefaultKeyBindings()
{
    keyBindings_[DIK_D] = 0; // レーン0
    keyBindings_[DIK_F] = 1; // レーン1
    keyBindings_[DIK_J] = 2; // レーン2
    keyBindings_[DIK_K] = 3; // レーン3
}
