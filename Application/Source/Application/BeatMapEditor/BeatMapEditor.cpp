#include "BeatMapEditor.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/Debug.h>
#include <System/Input/Input.h>

#include <Application/BeatMapLoader/BeatMapLoader.h>


void BeatMapEditor::Initialize()
{
    input_ = Input::GetInstance();
    lineDrawer_ = LineDrawer::GetInstance();

    beatMapLoader_ = BeatMapLoader::GetInstance();
    editorCoordinate_.Initialize(Vector2(1280.0f/2.0f, 720.0f), 4); // 初期画面サイズとレーン数を設定


    currentBeatMapData_ = BeatMapData(); // 現在の譜面データを初期化
    isModified_ = false; // 譜面が変更されていない状態に初期化
    currentTime_ = 0.0f; // 現在の時間を初期化
    isPlaying_ = false; // 再生状態を初期化

    playSpeed_ = 1.0f; // 再生速度を初期化


    for2dCamera_.Initialize(CameraType::Orthographic, Vector2(1280.0f, 720.0f)); // 2Dカメラの初期化
    for2dCamera_.matProjection_ = Matrix4x4::Identity();
    for2dCamera_.matView_= Matrix4x4::Identity();
    for2dCamera_.UpdateMatrix(); // カメラの行列を更新
    lineDrawer_->SetCameraPtr2D(&for2dCamera_); // 2Dカメラをライン描画クラスに設定

    selectedNoteIndices_.clear(); // 選択中のノートインデックスをクリア

    snapInterval_ = 1.0f / 4.0f; // グリッドスナップの間隔を1/4拍に設定
    gridSnapEnabled_ = true; // グリッドスナップを有効に初期化

    const size_t kMaxNoteCount = 1 << 10; // 最大ノート数を設定 一旦1024個とする
    noteSprites_.clear();
    noteSprites_.resize(kMaxNoteCount); // ノートスプライトのリストを予約
    for (size_t i = 0; i < kMaxNoteCount; ++i)
    {
        auto sprite = std::make_unique<UISprite>();
        sprite->Initialize("NoteSprite_" + std::to_string(i));
        sprite->SetAnchor(Vector2(0.5f, 0.5f)); // ノートのアンカーを中央に設定
        sprite->SetSize(Vector2(50.0f, 25.0f)); // ノートのサイズを設定

        noteSprites_[i] = std::move(sprite); // スプライトをリストに格納
    }

    // レーンのスプライトを初期化
    laneSprites_.clear();
    float laneWidth = editorCoordinate_.GetLaneWidth();
    for (uint32_t i = 0; i < editorCoordinate_.GetLaneCount(); ++i)
    {
        auto laneSprite = std::make_unique<UISprite>();

        laneSprite->Initialize("LaneSprite_" + std::to_string(i));
        laneSprite->SetAnchor(Vector2(0.5f, 0.0f)); // レーンのアンカーを左下に設定
        laneSprite->SetSize(Vector2(laneWidth, 720.0f)); // レーンのサイズを設定
        laneSprite->SetPos(Vector2(editorCoordinate_.GetLaneLeftX(i) + laneWidth/2.0f, 0.0f)); // レーンの位置を設定
        laneSprite->SetColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));

        laneSprites_.push_back(std::move(laneSprite)); // スプライトをリストに格納
    }

}

void BeatMapEditor::Update(float _deltaTime)
{
#ifdef _DEBUG

    ImGui::Begin("Editor");
    {
        if (ImGui::Button("Load BeatMap"))
        {
            // 譜面データのロード処理
            LoadBeatMap("Resources/Data/Game/BeatMap/demo.json"); // 適切なパスに変更してください
        }

        float zoom = editorCoordinate_.GetZoom();
        if (ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.01f, 100.0f))
            editorCoordinate_.SetZoom(zoom);

    }
    ImGui::End();


#endif // _DEBUG

    // 入力処理
    HandleInput();

    // エディター状態の更新
    UpdateEditorState(_deltaTime);
}

void BeatMapEditor::Draw(const Camera* _camera)
{
    // エディターの描画処理
    Sprite::PreDraw();

    // laneの描画
    DrawLanes();
    // グリッドラインの描画
    DrawGridLines();

    // ノートの描画
    DrawNotes();


}


void BeatMapEditor::DrawNotes()
{
    // ノートの描画処理
    for (const auto& note : currentBeatMapData_.notes)
    {
        DrawNote(note);
    }

    noteIndex_ = 0; // 描画後にインデックスをリセット
}

void BeatMapEditor::DrawNote(const NoteData& _note)
{
    if (noteIndex_ >= noteSprites_.size())
    {
        Debug::Log("Exceeded maximum note sprites limit.\n");
        return; // 最大ノート数を超えた場合は描画しない
    }

    float noteX = editorCoordinate_.LaneToScreenX(_note.laneIndex);
    float noteY = editorCoordinate_.TimeToScreenY(_note.targetTime);

    noteSprites_[noteIndex_]->SetPos(Vector2(noteX, noteY));
    noteSprites_[noteIndex_]->Draw(); // ノートを描画

    ++noteIndex_;
}

void BeatMapEditor::DrawLanes()
{
    for (size_t i = 0; i < laneSprites_.size(); ++i)
    {
        laneSprites_[i]->Draw(); // レーンを描画
    }
}

void BeatMapEditor::DrawGridLines()
{
    auto gridY = editorCoordinate_.GetGridLinesY(currentBeatMapData_.bpm, 4); // グリッドラインのY座標を取得

    // グリッドラインはLineで描画
    float gridLeftX = editorCoordinate_.GetEditAreaX();
    float gridRightX = gridLeftX + editorCoordinate_.GetEditAreaWidth();

    for (float y : gridY)
    {
        lineDrawer_->RegisterPoint(Vector2(gridLeftX, y), Vector2(gridRightX, y), Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    }


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
    // マウスホバーによる選択
    for (size_t i = 0; i < noteSprites_.size(); ++i) // 現在描画中のノート数まで
    {
        if (noteSprites_[i]->IsMousePointerInside())
        {
            // ホバー時の視覚的フィードバック
            noteSprites_[i]->SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色でハイライト

            // クリック検出
            if (input_->IsMouseTriggered(0))
            {
                bool multiSelect = input_->IsKeyPressed(DIK_LSHIFT);
                SelectNote(i, multiSelect);
            }
        }
        else
        {
            // 通常の色に戻す
            bool isSelected = IsNoteSelected(i);
            noteSprites_[i]->SetColor(isSelected ?
                Vector4(0.0f, 1.0f, 0.0f, 1.0f) : // 選択中は緑
                Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // 通常は白
        }
    }
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

bool BeatMapEditor::IsNoteSelected(uint32_t _noteIndex) const
{
    return std::find(selectedNoteIndices_.begin(), selectedNoteIndices_.end(), _noteIndex) != selectedNoteIndices_.end();
}
