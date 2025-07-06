#include "CommandHistory.h"

void CommandHistory::ExecuteCommand(std::unique_ptr<ICommand> _command)
{
    if (!_command)
        return;

    // コマンドを実行
    _command->Execute();
    // 実行したコマンドをundoスタックに追加
    undoStack_.push_back(std::move(_command));

    // undoスタックのサイズを制限
    while (undoStack_.size() > maxUndoSize_)
    {
        // 最大サイズを超えた場合、最も古いコマンドを削除
        undoStack_.pop_front();
    }

    // redoスタックをクリア
    redoStack_.clear();
}

bool CommandHistory::CanUndo() const
{
    return !undoStack_.empty();
}

bool CommandHistory::CanRedo() const
{
    return !redoStack_.empty();
}

void CommandHistory::Undo()
{
    // Undoが可能かチェック
    if (!CanUndo()) return;

    // 最後のコマンドを取得
    auto command = std::move(undoStack_.back());
    undoStack_.pop_back();
    // コマンドを元に戻す
    command->Undo();
    // redoスタックに追加
    redoStack_.push_back(std::move(command));
}

void CommandHistory::Redo()
{
    // Redoが可能かチェック
    if (!CanRedo()) return;

    // 最後のコマンドを取得
    auto command = std::move(redoStack_.back());
    redoStack_.pop_back();
    // コマンドを実行
    command->Execute();
    // undoスタックに追加
    undoStack_.push_back(std::move(command));
}

void CommandHistory::Clear()
{
    undoStack_.clear();
    redoStack_.clear();
}
