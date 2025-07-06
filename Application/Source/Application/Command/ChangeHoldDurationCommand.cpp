#include "ChangeHoldDurationCommand.h"

ChangeHoldDurationCommand::ChangeHoldDurationCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex, float _oldHoldDuration, float _newHoldDuration) :
    beatMapEditor_(_beatMapEditor),
    noteIndex_(_noteIndex),
    oldHoldDuration_(_oldHoldDuration),
    newHoldDuration_(_newHoldDuration)
{
}

void ChangeHoldDurationCommand::Execute()
{
    if (!beatMapEditor_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    // ノートの持続時間を変更
    beatMapEditor_->SetNoteDuration(noteIndex_, newHoldDuration_);
}

void ChangeHoldDurationCommand::Undo()
{
    if (!beatMapEditor_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    // 元の持続時間に戻す
    beatMapEditor_->SetNoteDuration(noteIndex_, oldHoldDuration_);
}
