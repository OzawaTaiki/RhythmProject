#include "MoveNoteCommand.h"

#include <Application/BeatMapEditor/BeatMapEditor.h>

MoveNoteCommand::MoveNoteCommand(BeatMapEditor* _beatMapEditor, const std::vector<size_t>& _noteIndices, float _deltaTime):
    beatMapEditor_(_beatMapEditor),
    noteIndices_(_noteIndices),
    deltaTime_(_deltaTime)
{
    originalTimes_.reserve(noteIndices_.size());
    noteIdentifiers_.reserve(noteIndices_.size());

    for (const auto& index : noteIndices_)
    {
        const NoteData& note = _beatMapEditor->GetNoteAt(index);
        originalTimes_.push_back(note.targetTime);
        noteIdentifiers_.push_back({ note.targetTime, note.laneIndex });
    }
}

void MoveNoteCommand::Execute()
{
    if (!beatMapEditor_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }

    // ノートの時間を更新
    for (uint32_t i = 0; i < noteIndices_.size(); ++i)
    {
        size_t index = noteIndices_[i];
        float newTime = originalTimes_[i] + deltaTime_;
        beatMapEditor_->SetNoteTime(index, newTime);
    }
    beatMapEditor_->SortNotesByTime(); // ノートを時間順にソート
}

void MoveNoteCommand::Undo()
{
    if (!beatMapEditor_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    // 元の時間に戻す
    for (uint32_t i = 0; i < noteIndices_.size(); ++i)
    {
        float currentTime = originalTimes_[i] + deltaTime_; // 移動後の時間
        uint32_t index = noteIdentifiers_[i].laneIndex; // レーンインデックス

        int32_t noteIndex = beatMapEditor_->FindNoteAtTime(noteIdentifiers_[i].laneIndex, currentTime);
        if (noteIndex >= 0)
        {
            // ノートが見つかった場合、元の時間に戻す
            beatMapEditor_->SetNoteTime(static_cast<size_t>(noteIndex), originalTimes_[i]);
        }
    }
    beatMapEditor_->SortNotesByTime(); // ノートを時間順にソート
}
