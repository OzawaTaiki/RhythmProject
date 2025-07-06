#include "PlaceNoteCommand.h"

#include <Application/BeatMapEditor/BeatMapEditor.h>

PlaceNoteCommand::PlaceNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration) :
    beatMapEditor_(_beatMapEditor),
    laneIndex_(_laneIndex),
    targetTime_(_targetTime),
    noteType_(_noteType),
    holdDuration_(_holdDuration),
    placeNoteIndex_(SIZE_MAX)
{
}

void PlaceNoteCommand::Execute()
{
    if (!beatMapEditor_)
        return;

    // ノートを配置
    placeNoteIndex_ = beatMapEditor_->PlaceNote(laneIndex_, targetTime_, noteType_, holdDuration_);
}

void PlaceNoteCommand::Undo()
{
    if (!beatMapEditor_ )
        return;

    // ノートを削除
    beatMapEditor_->DeleteNote(static_cast<uint32_t>(placeNoteIndex_));
}
