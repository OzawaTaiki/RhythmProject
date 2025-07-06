#pragma once

class ICommand 
{

public:
    // デストラクタ
    virtual ~ICommand() = default;

    /// <summary> (Redo兼)
    /// </summary>
    virtual void Execute() = 0;

    /// <summary>
    /// 元に戻す
    /// </summary>
    virtual void Undo() = 0;

};