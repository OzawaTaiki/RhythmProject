#include "BeatMapEditor.h"

#include <Application/BeatMapLoader/BeatMapLoader.h>
#include <Debug/Debug.h>


void BeatMapEditor::Initialize()
{
    beatMapLoader_ = BeatMapLoader::GetInstance();
    editorCoordinate_.Initialize(Vector2(1280.0f, 720.0f), 4); // 初期画面サイズとレーン数を設定


    currentBeatMapData_ = BeatMapData(); // 現在の譜面データを初期化
    isModified_ = false; // 譜面が変更されていない状態に初期化
    currentTime_ = 0.0f; // 現在の時間を初期化
    isPlaying_ = false; // 再生状態を初期化

    playSpeed_ = 1.0f; // 再生速度を初期化

    selectedNoteIndices_.clear(); // 選択中のノートインデックスをクリア

    snapInterval_ = 1.0f / 4.0f; // グリッドスナップの間隔を1/4拍に設定
    gridSnapEnabled_ = true; // グリッドスナップを有効に初期化

    const size_t kMaxNoteCount = 1024; // 最大ノート数を設定
    noteSprites_.resize(kMaxNoteCount); // ノートスプライトのリストを予約

}

void BeatMapEditor::Update(float _deltaTime)
{
    // 入力処理
    HandleInput();

    // エディター状態の更新
    UpdateEditorState(_deltaTime);
}

void BeatMapEditor::Draw(const Camera* _camera)
{
    // エディターの描画処理

    // ノートの描画
    //DrawNotes();


}

void BeatMapEditor::Finalize()
{
    // 保存確認
    if (isModified_)
    {
        Debug::Log("You have unsaved changes. Please save your beatmap before exiting.\n");
        // ここで保存ダイアログを表示するなどの処理を追加できます
    }

    // ここで必要なクリーンアップ処理を行います
    beatMapLoader_ = nullptr; // BeatMapLoaderのインスタンスを解放
}

void BeatMapEditor::LoadBeatMap(const std::string& _beatMapPath)
{
    if (!beatMapLoader_)
    {
        return;
    }

     auto future = beatMapLoader_->LoadBeatMap(_beatMapPath);

    // ロードが完了するまで待機
    if (future.get())
    {
        currentBeatMapData_ = beatMapLoader_->GetLoadedBeatMapData();
        currentFilePath_ = _beatMapPath; // 現在のファイルパスを更新
    }
    else
    {
        // ロード失敗時のエラーメッセージを表示
        std::string errorMessage = beatMapLoader_->GetErrorMessage();
        Debug::Log("Error loading beatmap: " + errorMessage + "\n");
    }

}

void BeatMapEditor::SaveBeatMap(const std::string& _beatMapPath)
{
    // 譜面データを保存
    // のち実装
}

void BeatMapEditor::CreateNewBeatMap(const std::string& _audioFilePath)
{
    currentBeatMapData_ = BeatMapData(); // 新しい譜面データを初期化
    currentBeatMapData_.audioFilePath = _audioFilePath; // 音声ファイルパスを設定
    currentBeatMapData_.bpm = 120.0f; // デフォルトBPMを設定
    currentBeatMapData_.offset = 0.0f; // デフォルトオフセットを設定

    currentFilePath_ = "";
    isModified_ = true; // 新規作成なので変更された状態にする
    currentTime_ = 0.0f; // 現在の時間を初期化
    isPlaying_ = false; // 再生状態を初期化
    selectedNoteIndices_.clear(); // 選択中のノートインデックスをクリア

}

void BeatMapEditor::PlaceNote(uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration)
{
    if (_laneIndex >= editorCoordinate_.GetLaneCount())
    {
        Debug::Log("Invalid lane index: " + std::to_string(_laneIndex) + "\n");
        return;
    }

    if (gridSnapEnabled_) {
        _targetTime = editorCoordinate_.SnapTimeToGrid(_targetTime, currentBeatMapData_.bpm, 4);
    }

    uint32_t exisingNote = FindNoteAtTime(_laneIndex, _targetTime, 0.05f);
    if (exisingNote >= 0) {
        Debug::Log("Note already exists at this position\n");
        return;  // 重複なら配置しない
    }

    // ノートを配置
    NoteData newNote;
    newNote.laneIndex = _laneIndex;
    newNote.targetTime = _targetTime;
    newNote.noteType = _noteType;
    newNote.holdDuration = _holdDuration;
    currentBeatMapData_.notes.push_back(newNote);
    isModified_ = true; // 譜面が変更されたフラグを立てる
    Debug::Log("Placed note at lane " + std::to_string(_laneIndex) + " at time " + std::to_string(_targetTime) + "\n");
}

void BeatMapEditor::DeleteNote(uint32_t _noteIndex)
{
    if (_noteIndex >= currentBeatMapData_.notes.size())
    {
        Debug::Log("Invalid note index: " + std::to_string(_noteIndex) + "\n");
        return;
    }

    // ノートを削除
    currentBeatMapData_.notes.erase(currentBeatMapData_.notes.begin() + _noteIndex);
    isModified_ = true; // 譜面が変更されたフラグを立てる

    // 選択状態更新
    auto it = std::find(selectedNoteIndices_.begin(), selectedNoteIndices_.end(), _noteIndex);
    if (it != selectedNoteIndices_.end()) {
        selectedNoteIndices_.erase(it);
    }

    // インデックス調整
    for (auto& index : selectedNoteIndices_) {
        if (index > _noteIndex) {
            index--;
        }
    }
    Debug::Log("Deleted note at index " + std::to_string(_noteIndex) + "\n");
}

void BeatMapEditor::SelectNote(uint32_t _noteIndex, bool _multiSelect)
{
    if (_noteIndex >= currentBeatMapData_.notes.size()) {
        return;
    }

    if (!_multiSelect) {
        selectedNoteIndices_.clear();
    }

    auto it = std::find(selectedNoteIndices_.begin(), selectedNoteIndices_.end(), _noteIndex);
    if (it == selectedNoteIndices_.end()) {
        selectedNoteIndices_.push_back(_noteIndex);
    }

    Debug::Log("Selected note at index " + std::to_string(_noteIndex) + "\n");
}


void BeatMapEditor::ClearSelection()
{
    selectedNoteIndices_.clear(); // 選択中のノートインデックスをクリア
}

void BeatMapEditor::MoveSelectedNote(float _newTime)
{
    if (selectedNoteIndices_.empty())
    {
        return;
    }

    // グリッドスナップ
    if (gridSnapEnabled_)
    {
        _newTime = editorCoordinate_.SnapTimeToGrid(_newTime, currentBeatMapData_.bpm, 4);
    }

    for (size_t index : selectedNoteIndices_)
    {
        if (index < currentBeatMapData_.notes.size())
        {
            currentBeatMapData_.notes[index].targetTime = _newTime;
        }
    }

    isModified_ = true;
    SortNotesByTime();
}
void BeatMapEditor::HandleInput()
{
    // 入力処理の実装
}

void BeatMapEditor::UpdateEditorState(float _deltaTime)
{
    if (isPlaying_)
    {
        currentTime_ += _deltaTime * playSpeed_;
    }

    editorCoordinate_.SetScrollOffset(currentTime_ - 2.0f);
}

int32_t BeatMapEditor::FindNoteAtTime(uint32_t _laneIndex, float _targetTime, float _tolerance) const
{
    for (size_t i = 0; i < currentBeatMapData_.notes.size(); ++i)
    {
        const NoteData& note = currentBeatMapData_.notes[i];
        if (note.laneIndex == _laneIndex &&
            std::abs(note.targetTime - _targetTime) <= _tolerance)
        {
            return static_cast<int32_t>(i); // ノートのインデックスを返す
        }
    }

    return static_cast<int32_t>(-1); // 見つからなかった場合は無効なインデックスを返す
}

void BeatMapEditor::SortNotesByTime()
{
    std::sort(currentBeatMapData_.notes.begin(), currentBeatMapData_.notes.end(),
        [](const NoteData& a, const NoteData& b) {
            return a.targetTime < b.targetTime;
        });
    isModified_ = true; // ソート後も変更されたフラグを立てる
}

void BeatMapEditor::DrawNotes()
{
    // ノートの描画処理
    for (const auto& note : currentBeatMapData_.notes)
    {
        note.laneIndex;
        note.targetTime;
        // ノートの描画ロジックを実装
        // 例: note.laneIndex, note.targetTime, note.noteType などを使用して描画

    }
}

void BeatMapEditor::DrawNote(const NoteData& _note) const
{
    float noteX = editorCoordinate_.LaneToScreenX(_note.laneIndex);
    float noteY = editorCoordinate_.TimeToScreenY(_note.targetTime);

    for (size_t i = 0; i < noteSprites_.size(); ++i)
    {
        if (noteSprites_[i].GetLabel() == _note.noteType)
        {
            noteSprites_[i].SetPos(Vector2(noteX, noteY));
            noteSprites_[i].Draw();
            return; // 描画が完了したら終了
        }
    }

}
