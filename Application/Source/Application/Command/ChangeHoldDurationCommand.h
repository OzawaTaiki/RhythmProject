#pragma once

#include <Application/Command/ICommand.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

class ChangeHoldDurationCommand : public ICommand
{
public:
    ChangeHoldDurationCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex, float _oldHoldDuration,float _newHoldDuration);
    void Execute() override;
    void Undo() override;

private:
    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ
    uint32_t noteIndex_ = 0; // 持続時間を変更するノートのインデックス
    float newHoldDuration_ = 0.0f; // 新しい持続時間
    float oldHoldDuration_ = 0.0f; // 元の持続時間を保存
};