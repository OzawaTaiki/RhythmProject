#include "DeleteNoteCommand.h"

DeleteNoteCommand::DeleteNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex):
    beatMapEditor_(_beatMapEditor),
    noteIndex_({ _noteIndex })
{
}

DeleteNoteCommand::DeleteNoteCommand(BeatMapEditor* _beatMapEditor, std::vector<uint32_t> _noteIndex) :
    beatMapEditor_(_beatMapEditor),
    noteIndex_(_noteIndex)
{
}

void DeleteNoteCommand::Execute()
{
    if (!beatMapEditor_)
    {
        return;
    }

    deletedNoteData_.clear();

    std::vector<NoteData> deletedNotes;
    // ノートを削除
    for(uint32_t index : noteIndex_)
    {
        deletedNotes.push_back(beatMapEditor_->GetNoteAt(index));
    }
    for (NoteData& note : deletedNotes)
    {
        // 削除したノートのデータを保存
        deletedNoteData_.push_back(beatMapEditor_->DeleteNote(note.laneIndex, note.targetTime));
    }
}

void DeleteNoteCommand::Undo()
{
    if (!beatMapEditor_)
    {
        return;
    }

    for (const auto& note : deletedNoteData_)
    {
        // 削除したノートを元に戻す
        beatMapEditor_->InsertNote(note);
    }
}
