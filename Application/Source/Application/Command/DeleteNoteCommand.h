#pragma once

#include <Application/Command/ICommand.h>

#include <Application/BeatMapEditor/BeatMapEditor.h>

class DeleteNoteCommand : public ICommand
{
public:

    DeleteNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _noteIndex);

    void Execute() override;
    void Undo() override;


private:

    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ
    uint32_t noteIndex_ = 0; // 削除するノートのインデックス
    NoteData deletedNoteData_; // 削除したノートのデータを保存
};