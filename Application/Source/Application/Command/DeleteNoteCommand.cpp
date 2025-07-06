#include "DeleteNoteCommand.h"

DeleteNoteCommand::DeleteNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex):
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
    // ノートを削除
    deletedNoteData_ = beatMapEditor_->DeleteNote(noteIndex_);


}

void DeleteNoteCommand::Undo()
{
    if (!beatMapEditor_)
    {
        return;
    }

    beatMapEditor_->InsertNote(deletedNoteData_); // 削除したノートを挿入して元に戻す
}
