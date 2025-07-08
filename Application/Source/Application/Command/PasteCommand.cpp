#include "PasteCommand.h"

PasteCommand::PasteCommand(BeatMapEditor* _beatMapEditor, const std::vector<NoteData>& _notesToPaste, float _pasteOffset):
    beatMapEditor_(_beatMapEditor),
    notesToPaste_(_notesToPaste),
    pasteOffset_(_pasteOffset)
{
}
void PasteCommand::Execute()
{
    if (!beatMapEditor_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    if (notesToPaste_.empty())
    {
        return; // コピーしたノートがない場合は何もしない
    }
    // ペーストオフセットを考慮してノートを配置
    for (const auto& note : notesToPaste_)
    {
        NoteData newNote = note;
        newNote.targetTime += pasteOffset_; // オフセットを適用
        size_t noteIndex = beatMapEditor_->InsertNote(newNote); // ノートを挿入
        if (noteIndex != SIZE_MAX) // ノートが正常に配置された場合
        {
            pastedNoteIndices_.push_back(noteIndex); // 配置したノートのインデックスを保存
        }
    }
}

void PasteCommand::Undo()
{
    if (!beatMapEditor_)
    {
        return; // BeatMapEditorが無効な場合は何もしない
    }
    // ペーストしたノートを削除
    for (int32_t i = 0; i < pastedNoteIndices_.size(); ++i)
    {
        size_t index = pastedNoteIndices_[i];
        beatMapEditor_->DeleteNote(index - i); // 消された分詰められているのでその分を考慮する
    }
    pastedNoteIndices_.clear(); // 削除後はインデックスをクリア
}
