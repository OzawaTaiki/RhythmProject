#pragma once

#include <Application/Command/ICommand.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

class PasteCommand : public ICommand
{
public:
    PasteCommand(BeatMapEditor* _beatMapEditor, const std::vector<NoteData>& _notesToPaste, float _pasteOffset = 0.0f);;
    void Execute() override;
    void Undo() override;
private:

    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ
    std::vector<NoteData> notesToPaste_; // コピーしたノートデータ
    std::vector<size_t> pastedNoteIndices_; // ペーストしたノートのインデックス
    float pasteOffset_ = 0.0f; // ペーストオフセット時間
};