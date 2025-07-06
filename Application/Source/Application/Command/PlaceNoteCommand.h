#pragma once

#include <Application/Command/ICommand.h>

#include <string>
#include <cstdint>

class BeatMapEditor;

class PlaceNoteCommand : public ICommand
{
public:
    PlaceNoteCommand(BeatMapEditor* _beatMapEditor, uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration = 0.0f);

    void Execute() override;
    void Undo() override;

private:
    BeatMapEditor* beatMapEditor_ = nullptr; // BeatMapEditorのポインタ

    // 配置するノートのデータ
    uint32_t laneIndex_ = 0; // レーンインデックス
    float targetTime_ = 0.0f; // ターゲット時間
    std::string noteType_ = "normal"; // ノートの種類 (normal, long)
    float holdDuration_ = 0.0f; // ホールドノートの持続時間

    size_t placeNoteIndex_ = 0; // 配置したノートのインデックス


};