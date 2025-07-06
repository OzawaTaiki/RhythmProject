#pragma once

#include <Application/Command/ICommand.h>


#include <vector>
#include <cstdint>

class BeatMapEditor; // 前方宣言
class MoveNoteCommand : public ICommand
{
public:
    /// <summary>
    /// 指定されたノートを指定時間分移動するコマンド
    /// </summary>
    /// <param name="_editor">BeatMapEditorのポインタ</param>
    /// <param name="_noteIndices">移動対象ノートのインデックスリスト</param>
    /// <param name="_deltaTime">移動量（秒）正数で未来方向</param>
    MoveNoteCommand(BeatMapEditor* _beatMapEditor, const std::vector<size_t>& _noteIndices, float _deltaTime);

    void Execute() override;
    void Undo() override;
private:
    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ


    std::vector<size_t> noteIndices_; // 移動するノートのインデックス
    std::vector<float> originalTimes_; // 元のノートの時間

    float deltaTime_ = 0.0f; // 移動する時間の差分

    struct NoteIdentifier
    {
        float time; // ノートの時間
        uint32_t laneIndex; // レーンインデックス
    };

    std::vector<NoteIdentifier> noteIdentifiers_; // ノートの識別子リスト

};