#include "BeatMapEditor.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/Debug.h>
#include <Debug/ImGuiHelper.h>
#include <System/Input/Input.h>
#include <System/Audio/AudioSystem.h>

#include <Application/BeatMapLoader/BeatMapLoader.h>
#include <Utility/FileDialog/FileDialog.h>
#include <Utility/StringUtils/StringUitls.h>

#include <fstream>

// TODO いろいろ
// ブリッジに重ねてノーツを置けてしまう
// zoom スクロール grid分割 全部キーボードあるいはマウスでできるように
// 流しながらノート入力したーい

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

    const size_t kMaxNoteCount = 1 << 6; // 最大ノート数を設定 一旦64個とする
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

    holdNoteEnd_.clear(); // ロングノートの終端スプライトをクリア
    holdNoteEnd_.reserve(kMaxNoteCount / 2); // 終端スプライトのリストを予約
    noteBridges_.clear(); // ノートとスプライトのブリッジをクリア
    noteBridges_.reserve(kMaxNoteCount / 2); // ブリッジのリストを予約
    for (size_t i = 0; i < kMaxNoteCount / 2; ++i)
    {
        auto longNoteEndSprite = std::make_unique<UISprite>();
        longNoteEndSprite->Initialize("LongNoteEndSprite_" + std::to_string(i));
        longNoteEndSprite->SetAnchor(Vector2(0.5f, 0.5f)); // ロングノートの終端のアンカーを中央に設定
        longNoteEndSprite->SetSize(Vector2(50.0f, 25.0f)); // ロングノートの終端のサイズを設定

        holdNoteEnd_.push_back(std::move(longNoteEndSprite)); // 終端スプライトをリストに格納


        auto bridgeSprite = std::make_unique<UISprite>();
        bridgeSprite->Initialize("NoteBridgeSprite_" + std::to_string(i));
        bridgeSprite->SetAnchor(Vector2(0.5f, 1.0f));// ブリッジのアンカーを中央上に設定
        bridgeSprite->SetSize(Vector2(40.0f, 25.0f)); // ブリッジのサイズを設定

        noteBridges_.push_back(std::move(bridgeSprite)); // ブリッジをリストに格納
    }

    // レーンのスプライトを初期化
    laneSprites_.clear();
    float laneWidth = editorCoordinate_.GetLaneWidth();
    for (uint32_t i = 0; i < editorCoordinate_.GetLaneCount(); ++i)
    {
        auto laneSprite = std::make_unique<UISprite>();

        laneSprite->Initialize("LaneSprite_" + std::to_string(i));
        laneSprite->SetAnchor(Vector2(0.5f, 0.0f)); // レーンのアンカーを中央下に設定
        laneSprite->SetSize(Vector2(laneWidth, 720.0f)); // レーンのサイズを設定
        laneSprite->SetPos(Vector2(editorCoordinate_.GetLaneLeftX(i) + laneWidth/2.0f, 0.0f)); // レーンの位置を設定
        laneSprite->SetColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));

        laneSprites_.push_back(std::move(laneSprite)); // スプライトをリストに格納
    }

    // playheadのスプライトを初期化
    playheadSprite_ = std::make_unique<UISprite>();
    playheadSprite_->Initialize("PlayheadSprite");
    playheadSprite_->SetTextureNameAndLoad("triangle.png"); // テクスチャを設定
    playheadSprite_->SetAnchor(Vector2(0.5f, 0.5f)); // 再生ヘッドのアンカーを中央下に設定
    playheadSprite_->SetSize(Vector2(20.0f, 20.0f)); // 再生ヘッドのサイズを設定
    playheadSprite_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // 再生ヘッドの色を赤に設定
    playheadSprite_->SetRotate(1.57f); // 再生ヘッドの回転を設定（PI/2ラジアン）

    float playheadsize = playheadSprite_->GetSize().x; // 再生ヘッドのサイズを取得
    Vector2 playheadPos = { editorCoordinate_.GetEditAreaX() - playheadsize / 2.0f, editorCoordinate_.TimeToScreenY(scrollOffset_) }; // 再生ヘッドの初期位置を設定
    playheadSprite_->SetPos(playheadPos);

    // ジャッジラインのスプライトを初期化
    judgeLineSprite_ = std::make_unique<UISprite>();
    judgeLineSprite_->Initialize("JudgeLineSprite");
    judgeLineSprite_->SetAnchor(Vector2(0.0f, 0.5f));// ジャッジラインのアンカーを左中央に設定
    float judgeLineXMargin = 30.0f; // ジャッジラインのXマージンを設定
    judgeLineSprite_->SetSize(Vector2(editorCoordinate_.GetEditAreaWidth() + judgeLineXMargin, 5.0f)); // ジャッジラインのサイズを設定
    judgeLineSprite_->SetPos(Vector2(editorCoordinate_.GetEditAreaX() - judgeLineXMargin / 2.0f, editorCoordinate_.TimeToScreenY(currentTime_))); // ジャッジラインの位置を設定
    judgeLineSprite_->SetColor(Vector4(0.8f, 0.2f, 0.2f, 1.0f)); // ジャッジラインの色を設定


    previewNoteSprite_ = std::make_unique<UISprite>();
    previewNoteSprite_->Initialize("PreviewNoteSprite");
    previewNoteSprite_->SetAnchor(Vector2(0.5f, 0.5f)); // プレビューのアンカーを中央に設定
    previewNoteSprite_->SetSize(Vector2(50.0f, 25.0f)); // ノートのサイズを設定

    previewBridgeSprite_ = std::make_unique<UISprite>();
    previewBridgeSprite_->Initialize("PreviewBridgeSprite");
    previewBridgeSprite_->SetAnchor(Vector2(0.5f, 1.0f)); // プレビューのアンカーを中央上に設定
    previewBridgeSprite_->SetSize(Vector2(40.0f, 25.0f)); // ブリッジのサイズを設定

    previewHoldEndSprite_ = std::make_unique<UISprite>();
    previewHoldEndSprite_->Initialize("PreviewHoldEndSprite");
    previewHoldEndSprite_->SetAnchor(Vector2(0.5f, 0.5f)); // プレビューのアンカーを中央に設定
    previewHoldEndSprite_->SetSize(Vector2(50.0f, 25.0f)); // ロングノート終端のサイズを設定


    // color
    normalNoteColor_.defaultColor = Vector4(0.31f, 0.76f, 0.97f, 1.0f); // デフォルトのノート色
    normalNoteColor_.hoverColor = Vector4(0.98f, 0.83f, 0.51f, 1.0f); // ホバー時の色
    normalNoteColor_.selectedColor = Vector4(0.97f, 0.98f, 0.01f, 1.0f); // 選択時の色

    longNoteColor_.defaultColor = Vector4(0.40f, 0.73f, 0.42f, 1.0f); // デフォルトのロングノート色
    longNoteColor_.hoverColor = Vector4(0.98f, 0.83f, 0.51f, 1.0f); // ホバー時の色
    longNoteColor_.selectedColor = Vector4(0.97f, 0.98f, 0.01f, 1.0f); // 選択時の色

    dummy_editArea_ = std::make_unique<UISprite>();
    dummy_editArea_->Initialize("DummySprite");
    dummy_editArea_->SetPos(Vector2(editorCoordinate_.GetEditAreaX(), 0));
    dummy_editArea_->SetAnchor(Vector2(0.0f, 0.0f));// ダミースプライトのアンカーを左上に設定
    dummy_editArea_->SetSize(Vector2(editorCoordinate_.GetEditAreaWidth(), 720)); // ダミースプライトのサイズを設定

    dummy_window_ = std::make_unique<UISprite>();
    dummy_window_->Initialize("DummyWindowSprite");
    dummy_window_->SetPos(Vector2(0, 0));
    dummy_window_->SetAnchor(Vector2(0.0f, 0.0f)); // ダミーウィンドウのアンカーを左上に設定
    dummy_window_->SetSize(Vector2(1280.0f, 720.0f)); // ダミーウィンドウのサイズを設定

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

    Sprite::PreDraw();

    // laneの描画
    DrawLanes();
    // グリッドラインの描画
    DrawGridLines();

    // ノートの描画
    DrawNotes();

    DrawPreviewNote();

    // 判定ラインの描画
    DrawJudgeLine();

    // 再生ヘッドの描画
    DrawPlayhead();

    DrawUI();

}

void BeatMapEditor::DrawNotes()
{
    noteIndex_ = 0; // ノートのインデックスをリセット
    holdNoteIndex_ = 0; // ブリッジのインデックスをリセット

    drawNoteIndices_.clear();

    // 可視範囲を取得
    float startTime, endTime;
    editorCoordinate_.GetVisibleTimeRange(startTime, endTime);

    // ノートの描画処理
    for (uint32_t drawNoteIndex = 0; drawNoteIndex < currentBeatMapData_.notes.size(); ++drawNoteIndex)
    {
        const NoteData& note = currentBeatMapData_.notes[drawNoteIndex];
        //noteが画面内にあるかチェック
        if ((note.targetTime < startTime ||
            note.targetTime > endTime) &&
            (note.targetTime + note.holdDuration < startTime ||
            note.targetTime + note.holdDuration > endTime) )
        {
            // 可視範囲
            float visibleTimeRange = endTime - startTime;
            // ノートのホールド時間が可視範囲より小さい場合は描画しない
            if (visibleTimeRange > note.holdDuration)
                continue;
        }

        DrawNote(note);

        // ノートのインデックスを記録
        drawNoteIndices_.push_back(drawNoteIndex);
    }

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


    if(_note.noteType =="hold")
    {
        Vector4 color = noteSprites_[noteIndex_]->GetColor();
        // ブリッジの描画
        {
            noteBridges_[holdNoteIndex_]->SetPos(Vector2(noteX, noteY)); // ブリッジの位置を設定
            noteBridges_[holdNoteIndex_]->SetColor(color);
            noteBridges_[holdNoteIndex_]->SetSize(Vector2(40.0f, editorCoordinate_.GetPixelsPerSecond() * _note.holdDuration)); // ブリッジのサイズを設定
            noteBridges_[holdNoteIndex_]->Draw(); // ブリッジを描画
        }

        // 終端ノートの描画
        {
            holdNoteEnd_[holdNoteIndex_]->SetPos(Vector2(noteX, noteY - editorCoordinate_.GetPixelsPerSecond() * _note.holdDuration)); // 終端ノートの位置を設定
            holdNoteEnd_[holdNoteIndex_]->SetColor(color); // 終端ノートの色を設定
            holdNoteEnd_[holdNoteIndex_]->Draw(); // 終端ノートを描画
        }

        ++holdNoteIndex_; // ブリッジのインデックスをインクリメント
    }

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
    auto gridY = editorCoordinate_.GetGridLinesY(currentBeatMapData_.bpm, static_cast<int>(1.0f / snapInterval_)); // グリッドラインのY座標を取得

    // グリッドラインはLineで描画
    float gridLeftX = editorCoordinate_.GetEditAreaX();
    float gridRightX = gridLeftX + editorCoordinate_.GetEditAreaWidth();

    for (float y : gridY)
    {
        lineDrawer_->RegisterPoint(Vector2(gridLeftX, y), Vector2(gridRightX, y), Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    }
}

void BeatMapEditor::DrawJudgeLine()
{
    float judgeLineXMargin = 30.0f; // ジャッジラインのXマージンを設定
    Vector2 pos = Vector2(editorCoordinate_.GetEditAreaX() - judgeLineXMargin / 2.0f, editorCoordinate_.TimeToScreenY(scrollOffset_));
    if (isPlaying_)
        pos.y = editorCoordinate_.TimeToScreenY(currentTime_);
    judgeLineSprite_->SetPos(pos);

    judgeLineSprite_->Draw();
}

void BeatMapEditor::DrawPlayhead()
{
    // 再生ヘッドの位置を更新
    float playheadX = editorCoordinate_.GetEditAreaX() - playheadSprite_->GetSize().x / 2.0f; // 再生ヘッドのX座標を設定
    float playheadY = editorCoordinate_.TimeToScreenY(currentTime_); // 再生ヘッドのY座標を設定
    playheadSprite_->SetPos(Vector2(playheadX, playheadY)); // 再生ヘッドの位置を設定

    playheadSprite_->Draw();

    // ラインの描画
    float lineY = playheadY;
    float lineLeftX = editorCoordinate_.GetEditAreaX();
    float lineRightX = lineLeftX + editorCoordinate_.GetEditAreaWidth();
    lineDrawer_->RegisterPoint(Vector2(lineLeftX, lineY), Vector2(lineRightX, lineY), Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // 再生ヘッドのラインを描画


}

void BeatMapEditor::DrawUI()
{
#ifdef _DEBUG

    FileFilterBuilder filterBuilder;
    filterBuilder.AddSeparateExtensions(FileFilterBuilder::FilterType::DataFiles);
    static std::string filter  = filterBuilder.Build();

    ImGui::Begin("BeatMap Editor");
    {
        //ImGuiHelper::InputText("File Path", currentFilePath_); // ファイルパスの入力フィールド

        if (ImGui::Button("Load BeatMap"))
        {
            currentFilePath_ = FileDialog::OpenFile((filter));
            LoadBeatMap(currentFilePath_); // 譜面のロード
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(currentFilePath_ == "");
        {
            if (ImGui::Button("Save BeatMap"))
            {
                currentFilePath_ = FileDialog::SaveFile(filter);
                ImGui::OpenPopup("Save Confirmation");
                //SaveBeatMap(currentFilePath_); // 譜面の保存
            }
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("New BeatMap"))
        {
            std::string newFilePath = FileDialog::CreateFile(filter);
            CreateNewBeatMap(newFilePath, "");
        }
        if (ImGui::BeginPopupModal("Save Confirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("SavePath : %s", currentFilePath_.c_str());
            ImGui::Separator();

            ImGuiHelper::InputText("Title", currentBeatMapData_.title); // タイトルの入力フィールド
            ImGui::DragFloat("BPM", &currentBeatMapData_.bpm, 0.1f, 0.1f, 1000.0f); // BPMの入力フィールド
            ImGui::DragFloat("Offset", &currentBeatMapData_.offset, 0.01f, -100.0f, 100.0f); // オフセットの入力フィールド

            if(ImGui::Button("ok"))
            {
                SaveBeatMap(currentFilePath_); // 譜面の保存
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if(ImGui::CollapsingHeader("Information", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SeparatorText("BeatMap Info");
            ImGui::Text("Current File: %s", currentFilePath_.empty() ? "None" : currentFilePath_.c_str());
            ImGui::Text("BPM: %.2f", currentBeatMapData_.bpm);
            ImGui::Text("Offset: %.2f", currentBeatMapData_.offset);
            ImGui::Text("Notes Count: %zu", currentBeatMapData_.notes.size());

            ImGui::SeparatorText("Editor State");
            ImGui::Text("Current Time: %.2f", currentTime_);
            ImGui::Text("Is Playing: %s", isPlaying_ ? "Yes" : "No");
            ImGui::Text("Scroll Offset: %.2f", scrollOffset_);
            ImGui::Text("Selected Notes: %zu", selectedNoteIndices_.size());
            ImGui::Text("Grid Snap: %s", gridSnapEnabled_ ? "Enabled" : "Disabled");

            ImGui::Text("Play Speed: %.2f", playSpeed_);
        }


        ImGui::DragFloat("Current Time", &currentTime_, 0.01f, 0.0f, 1000.0f);
        ImGui::DragFloat("Play Speed", &playSpeed_, 0.01f, 0.1f, 10.0f);
        static float zoom = 0;
        zoom = editorCoordinate_.GetZoom();
        if(ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f))
        {
            editorCoordinate_.SetZoom(zoom); // ズームレベルを設定
        }
        if (ImGui::Button("Play"))
        {
            isPlaying_ = !isPlaying_; // 再生/一時停止の切り替え
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            isPlaying_ = false; // 停止
            currentTime_ = 0.0f; // 時間をリセット
        }
        ImGui::Checkbox("Grid Snap", &gridSnapEnabled_); // グリッドスナップのチェックボックス
        // グリッドスナップの間隔を選択するラジオボタン
        static int snapIntervalIndex = 2;
        ImGui::RadioButton("1/1", &snapIntervalIndex, 0);
        ImGui::RadioButton("1/2", &snapIntervalIndex, 1);
        ImGui::RadioButton("1/4", &snapIntervalIndex, 2);
        ImGui::RadioButton("1/8", &snapIntervalIndex, 3);
        ImGui::RadioButton("1/16", &snapIntervalIndex, 4);
        snapInterval_ = 1.0f / std::powf(2.0f, static_cast<float>(snapIntervalIndex));

        ImGui::SeparatorText("Music Control");
        if (ImGui::Button("Load Music"))
        {
            std::string musicFilePath = FileDialog::OpenFile("Audio Files (*.wav;)\0*.wav;\0");
            if (!musicFilePath.empty())
            {
                // 音楽ファイルのパスを設定
                currentBeatMapData_.audioFilePath = musicFilePath;
                // 音楽をロード
                musicSoundInstance_ = AudioSystem::GetInstance()->Load(musicFilePath);
            }
        }
        ImGui::BeginDisabled(!musicSoundInstance_ ); // 音楽がロードされていない場合は無効化
        {
            if(ImGui::Button("Play Begin"))
            {
                RestartMusic();
            }
            ImGui::SameLine();

            if (ImGui::Button("Play Curren Time"))
            {
                PlayMusic();
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(!musicVoiceInstance_|| !musicVoiceInstance_->IsPlaying()); // 音楽が再生中でない場合は無効化
            {
                if (ImGui::Button("Stop Music"))
                {
                    StopMusic();
                }
            }
            ImGui::EndDisabled(); // 無効化終了
        }
        ImGui::EndDisabled();



        ImGui::Text("Music File : %s", currentBeatMapData_.audioFilePath.empty() ? "None" : currentBeatMapData_.audioFilePath.c_str());
        ImGui::Text("Music Duration: %.2f seconds", musicSoundInstance_ ? musicSoundInstance_->GetDuration() : 0.0f); // 音楽の再生時間を表示
        if (ImGui::DragFloat("Volume", &volume_, 0.01f, 0.0f, 1.0f)) // 音量の入力フィールド
        {
            if (musicVoiceInstance_)
                musicVoiceInstance_->SetVolume(volume_);
        }

    }
    ImGui::End();

#endif // _DEBUG
}

void BeatMapEditor::DrawPreviewNote()
{
    if (currentEditorMode_ == EditorMode::PlaceNormalNote ||
        currentEditorMode_ == EditorMode::PlaceLongNote)
    {
        Vector2 mousePos = input_->GetMousePosition(); // マウスの位置を取得

        // プレビューのノートを描画
        int32_t laneIndex = editorCoordinate_.ScreenXToLane(mousePos.x); // マウスのX座標をレーンに変換
        if (laneIndex < 0 || laneIndex >= static_cast<int32_t>(editorCoordinate_.GetLaneCount()))
        {
            return; // レーン外なら描画しない
        }
        float previewX = editorCoordinate_.LaneToScreenX(laneIndex); // レーンのX座標を取得
        float previewY = mousePos.y;
        previewNoteSprite_->SetPos(Vector2(previewX, previewY));
        Vector4 color;
        if (currentEditorMode_ == EditorMode::PlaceNormalNote)
        {
            color = normalNoteColor_.defaultColor; // ノーマルノートの色を設定
            color.w = previewAlpha_; // アルファ値を設定
            previewNoteSprite_->SetColor(color);
        }
        else if (currentEditorMode_ == EditorMode::PlaceLongNote)
        {
            color = longNoteColor_.defaultColor; // ロングノートの色を設定
            color.w = previewAlpha_; // アルファ値を設定
            previewNoteSprite_->SetColor(color);

            if (isCreatingLongNote_)
            {
                float posX = editorCoordinate_.LaneToScreenX(longNoteStartLane_);
                float posY = editorCoordinate_.TimeToScreenY(longNoteStartTime_);
                previewNoteSprite_->SetPos(Vector2(posX, posY));

                // ロングノートの終端のプレビューを描画
                float holdDuration = editorCoordinate_.ScreenYToTime(mousePos.y) - editorCoordinate_.ScreenYToTime(posY);
                if (holdDuration > 0.0f)
                {
                    previewHoldEndSprite_->SetPos(Vector2(posX, previewY));
                    previewHoldEndSprite_->SetColor(color);
                    previewHoldEndSprite_->Draw();
                }
                // ブリッジのプレビューを描画
                previewBridgeSprite_->SetPos(Vector2(posX, posY));
                previewBridgeSprite_->SetColor(color);
                previewBridgeSprite_->SetSize(Vector2(40.0f, holdDuration * editorCoordinate_.GetPixelsPerSecond()));
                previewBridgeSprite_->Draw();

            }
        }
        previewNoteSprite_->Draw();
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
        Reset(); // エディターの状態をリセット
        currentBeatMapData_ = beatMapLoader_->GetLoadedBeatMapData();
        currentFilePath_ = _beatMapPath; // 現在のファイルパスを更新

        std::string musicFilePath = currentBeatMapData_.audioFilePath;
        if (!musicFilePath.empty())
        {
            // 音声ファイルのパスが設定されている場合は、音声をロード
            musicSoundInstance_ = AudioSystem::GetInstance()->Load(musicFilePath);
        }
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
    std::string filePath = _beatMapPath;
    std::string extension = StringUtils::GetExtension(filePath);
    if (extension != ".json")
    {
        filePath += ".json"; // 拡張子がない場合は.jsonを追加
    }

    json j;
    j["title"] = currentBeatMapData_.title;
    j["artist"] = currentBeatMapData_.artist;
    j["audioFilePath"] = currentBeatMapData_.audioFilePath;
    j["bpm"] = currentBeatMapData_.bpm;
    j["offset"] = currentBeatMapData_.offset;
    j["difficultyLevel"] = currentBeatMapData_.difficultyLevel;

    for (const auto& note : currentBeatMapData_.notes)
    {
        json noteJson;
        noteJson["laneIndex"] = note.laneIndex;
        noteJson["targetTime"] = note.targetTime;
        noteJson["noteType"] = note.noteType;
        noteJson["holdDuration"] = note.holdDuration;
        j["notes"].push_back(noteJson);
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        Debug::Log("Failed to open file for saving: " + filePath + "\n");
        return;
    }

    outFile << j.dump(4); // JSONを整形して書き出し
    outFile.close();
    currentFilePath_ = filePath; // 現在のファイルパスを更新
    isModified_ = false; // 保存後は変更されていない状態にする
    Debug::Log("Beatmap saved successfully to: " + filePath + "\n");

}

void BeatMapEditor::CreateNewBeatMap(const std::string& _filePath, const std::string& _audioFilePath)
{
    Reset();

    currentBeatMapData_ = BeatMapData(); // 新しい譜面データを初期化
    currentBeatMapData_.audioFilePath = _audioFilePath; // 音声ファイルパスを設定
    currentBeatMapData_.bpm = 120.0f; // デフォルトBPMを設定
    currentBeatMapData_.offset = 0.0f; // デフォルトオフセットを設定

    currentFilePath_ = _filePath;
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
        int divi = static_cast<int>( 1.0f / snapInterval_);
        _targetTime = editorCoordinate_.SnapTimeToGrid(_targetTime, currentBeatMapData_.bpm, divi);
        _holdDuration = editorCoordinate_.SnapTimeToGrid(_holdDuration, currentBeatMapData_.bpm, divi);
    }

    int32_t exisingNote = FindNoteAtTime(_laneIndex, _targetTime, 0.05f);
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

    lastSelectedNoteIndex_ = _noteIndex; // 最後に選択されたノートのインデックスを保存
    Debug::Log("Selected note at index " + std::to_string(_noteIndex) + "\n");
}


void BeatMapEditor::ClearSelection()
{
    selectedNoteIndices_.clear(); // 選択中のノートインデックスをクリア
}

void BeatMapEditor::MoveSelectedNote( float _newTime)
{
    if (selectedNoteIndices_.empty())
    {
        return;
    }

    // グリッドスナップ
    if (gridSnapEnabled_)
    {
        _newTime = editorCoordinate_.SnapTimeToGrid(_newTime, currentBeatMapData_.bpm, static_cast<int>(1.0f / snapInterval_));
    }


    for (size_t index : selectedNoteIndices_)
    {
        if (index < currentBeatMapData_.notes.size())
        {
            float addTime = _newTime - currentBeatMapData_.notes[lastSelectedNoteIndex_].targetTime;
            currentBeatMapData_.notes[index].targetTime += addTime;
        }
    }

    isModified_ = true;
}

void BeatMapEditor::HandleInput()
{
    bool selected = false;
    // マウスがエディターエリア内にあるかチェック
    bool mouseInsideEditorArea = dummy_editArea_->IsMousePointerInside();
    if (!dummy_window_->IsMousePointerInside())
        return;// ダミーウィンドウ外なら何もしない

    if (mouseInsideEditorArea)
    {
        ImGui::Begin("BeatMap Editor");

        ImGui::Text("mouse Inside Editor Area");

        ImGui::End();
    }

    // 入力処理の実装
    // マウスホバーによる選択
    if(!isMovingSelectedNote_)
    {
        for (size_t i = 0; i < noteIndex_; ++i) // 現在描画中のノート数まで
        {
            uint32_t actualNoteIndex = drawNoteIndices_[i]; // 実際のノートインデックスを取得
            if (noteSprites_[i]->IsMousePointerInside())
            {
                // ホバー時の視覚的フィードバック
                if (currentBeatMapData_.notes[actualNoteIndex].noteType == "normal")
                {
                    noteSprites_[i]->SetColor(normalNoteColor_.hoverColor); // ノーマルノートのホバー色を設定
                }
                else if (currentBeatMapData_.notes[actualNoteIndex].noteType == "hold")
                {
                    noteSprites_[i]->SetColor(longNoteColor_.hoverColor); // ロングノートのホバー色を設定
                }
                else
                {
                    noteSprites_[i]->SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f)); // その他のノートは黄色に設定
                }

                // クリック検出
                if (input_->IsMouseTriggered(0))
                {
                    bool multiSelect = input_->IsKeyPressed(DIK_LSHIFT);
                    SelectNote(actualNoteIndex, multiSelect);
                    selected = true;
                    isMovingSelectedNote_ = true; // 選択したノートを移動可能にするフラグを立てる
                }
            }
            else
            {
                // 通常の色に戻す
                bool isSelected = IsNoteSelected(actualNoteIndex);
                Vector4 color;

                if (currentBeatMapData_.notes[actualNoteIndex].noteType == "normal")
                {
                    color = isSelected ? normalNoteColor_.selectedColor : normalNoteColor_.defaultColor; // ノーマルノートの色
                }
                else if (currentBeatMapData_.notes[actualNoteIndex].noteType == "hold")
                {
                    color = isSelected ? longNoteColor_.selectedColor : longNoteColor_.defaultColor; // ロングノートの色
                }
                else
                {
                    color = isSelected ? Vector4(1.0f, 1.0f, 0.0f, 1.0f) : Vector4(1.0f, 1.0f, 1.0f, 1.0f); // その他のノートは白色
                }

                noteSprites_[i]->SetColor(color); // ノートの色を設定
            }
        }
    }

    if (isMovingSelectedNote_ && input_->IsMouseReleased(0))
    {
        isMovingSelectedNote_ = false; // マウスの左ボタンが離されたとき、移動フラグを下ろす
        SortNotesByTime();
    }

    if (isMovingSelectedNote_)
    {
        // 選択されたノートを移動
        Vector2 mousePos = input_->GetMousePosition();

        float newTime = editorCoordinate_.ScreenYToTime(mousePos.y);
        MoveSelectedNote(newTime); // 選択されたノートの時間を更新
        selected = true; // ノートが移動されたので選択状態にする
    }


    for (size_t i = 0; i < holdNoteIndex_; ++i)
    {
        auto& note = holdNoteEnd_[i];

        if (note->IsMousePointerInside())
        {
            int32_t actualNoteIndex = GetNoteIndexFromHoldEnd(editorCoordinate_.ScreenXToLane(note->GetPos().x), editorCoordinate_.ScreenYToTime(note->GetPos().y));
            if (actualNoteIndex < 0)
                continue;

            int32_t drawNoteIndex = -1;
            for (int32_t j = 0; j < drawNoteIndices_.size(); ++j)
            {
                if (drawNoteIndices_[j] == actualNoteIndex)
                {
                    drawNoteIndex = j;
                    break;
                }
            }

            if (drawNoteIndex < 0)
                continue; // 描画されていないノートは無視

            // ホバー時の視覚的フィードバック
            if (currentBeatMapData_.notes[actualNoteIndex].noteType == "hold")
            {
                noteSprites_[drawNoteIndex]->SetColor(longNoteColor_.hoverColor); // ロングノートのホバー色を設定
            }

            if (input_->IsMouseTriggered(0))
            {
                isSelectingHoldEnd_ = true; // ホールド終端の選択フラグを立てる
                selectNoteIndex_ = actualNoteIndex; // 選択されたノートのインデックスを保存
                selected = true; // 選択状態にする
            }
        }
    }

    if (isSelectingHoldEnd_)
    {
        // ホールド終端が選択されている場合、マウスの位置に応じてノートを移動
        Vector2 mousePos = input_->GetMousePosition();
        float newTime = editorCoordinate_.ScreenYToTime(mousePos.y);
        float newHoldDuration = newTime - currentBeatMapData_.notes[selectNoteIndex_].targetTime; // 新しいホールド時間を計算
        currentBeatMapData_.notes[selectNoteIndex_].holdDuration = (std::max)(newHoldDuration, 0.0f); // ホールド時間を更新（負の値は許可しない）

        if (input_->IsMouseReleased(0)) // マウスの左ボタンが離されたとき
        {
            isSelectingHoldEnd_ = false; // ホールド終端の選択フラグを下ろす
            selectNoteIndex_ = -1; // 選択されたノートのインデックスをリセット
        }
    }


    // 選択していない
    if (!selected && mouseInsideEditorArea)
    {
        // クリックしたときリセット
        if (input_->IsMouseTriggered(0))
        {
            ClearSelection(); // クリックで選択をクリア
        }
    }

    // ホイールでスクロール
    float wheelDelta = input_->GetMouseWheel();
    if (wheelDelta != 0.0f)
    {
        if (!mouseInsideEditorArea)
        {
                // スクロール量に応じて時間を更新
            float addedTime = wheelDelta * 0.1f / editorCoordinate_.GetZoom();
            // スクロール速度を調整
            // ゆっくりスクロールできるように。
            if (input_->IsKeyPressed(DIK_LSHIFT))
            {
                addedTime *= 0.1f;
            }
            scrollOffset_ += addedTime; // 現在の時間を更新
            scrollOffset_ = (std::max)(scrollOffset_, 0.0f);
            editorCoordinate_.SetScrollOffset(scrollOffset_); // スクロールオフセットを更新
        }
        else
        {
            float addedZoom = wheelDelta * 0.1f; // ホイールの動きに応じてズームを調整
            if (input_->IsKeyPressed(DIK_LSHIFT))
            {
                addedZoom *= 0.1f;
            }
            editorCoordinate_.SetZoom(editorCoordinate_.GetZoom() + addedZoom); // ズームを更新

        }
    }


    if (input_->IsKeyTriggered(DIK_SPACE))
    {
        isPlaying_ = !isPlaying_; // スペースキーで再生/停止を切り替え
        if (isPlaying_)
            PlayMusic();
        else
            StopMusic();

        if (!isPlaying_)
        {
            // 停止時は現在の時間をスクロールオフセットに設定
            scrollOffset_ = currentTime_;
            editorCoordinate_.SetScrollOffset(currentTime_);
        }
    }

    // ホイールクリックでcurrentTimeをセット
    if (input_->IsMouseTriggered(2))
    {
        // マウスの位置を取得
        Vector2 mousePos = input_->GetMousePosition();
        // スクリーン座標から時間に変換
        float targetTime = editorCoordinate_.ScreenYToTime(mousePos.y);
        currentTime_ = (std::max)(targetTime, 0.0f); // 負の時間を防ぐ

    }

    //  Mode切り替え
    if (input_->IsKeyTriggered(DIK_1))
    {
        currentEditorMode_ = EditorMode::Select; // ノーマルモード
    }
    else if (input_->IsKeyTriggered(DIK_2))
    {
        currentEditorMode_ = EditorMode::PlaceNormalNote; // ホールドモード
    }
    else if (input_->IsKeyTriggered(DIK_3))
    {
        currentEditorMode_ = EditorMode::PlaceLongNote; // ホールドモード
    }
    else if (input_->IsKeyTriggered(DIK_4))
    {
        currentEditorMode_ = EditorMode::Delete; // 削除モード
    }

    if (input_->IsKeyTriggered(DIK_TAB))
    {
        int32_t mode = static_cast<int32_t>(currentEditorMode_);
        mode = (mode + 1) % static_cast<int32_t>(EditorMode::Count); // モードを循環
        currentEditorMode_ = static_cast<EditorMode>(mode); // 新しいモードを設定
    }

    if(currentEditorMode_==EditorMode::PlaceNormalNote ||
        currentEditorMode_ == EditorMode::PlaceLongNote)
    {
        if (input_->IsMouseTriggered(0)) // 左クリックでノートを配置
        {
            Vector2 mousePos = input_->GetMousePosition(); // マウスの位置を取得
            int32_t laneIndex = editorCoordinate_.ScreenXToLane(mousePos.x); // マウスのX座標をレーンに変換
            float targetTime = editorCoordinate_.ScreenYToTime(mousePos.y); // マウスのY座標を時間に変換
            if (currentEditorMode_ == EditorMode::PlaceNormalNote)
            {
                PlaceNote(laneIndex, targetTime, "normal"); // ノーマルノートを配置
            }
            else if (currentEditorMode_ == EditorMode::PlaceLongNote)
            {
                isCreatingLongNote_ = true; // ロングノートの作成フラグを立てる
                longNoteStartTime_ = targetTime; // ロングノートの開始時間を記録
                longNoteStartLane_ = laneIndex; // ロングノートの開始レーンを記録
            }
        }

        if (isCreatingLongNote_)
        {
            if (input_->IsMouseReleased(0)) // 左クリックを離したとき
            {
                Vector2 mousePos = input_->GetMousePosition(); // マウスの位置を取得

                float targetTime = editorCoordinate_.ScreenYToTime(mousePos.y); // マウスのY座標を時間に変換
                float holdDuration = targetTime - longNoteStartTime_; // ホールド時間を計算

                if (holdDuration > 0.001f) // ホールド時間が0.001秒以上の場合のみ配置
                {
                    PlaceNote(longNoteStartLane_, longNoteStartTime_, "hold", holdDuration); // ロングノートを配置x
                }
                isCreatingLongNote_ = false; // ロングノートの作成フラグを下ろす
            }
        }
    }
    if (currentEditorMode_ == EditorMode::Delete)
    {
        if (input_->IsMouseTriggered(0)) // 左クリックでノートを削除
        {
            for (size_t i = 0; i < noteIndex_; ++i) // 現在描画中のノート数まで
            {
                uint32_t actualNoteIndex = drawNoteIndices_[i]; // 実際のノートインデックスを取得
                if (noteSprites_[i]->IsMousePointerInside())
                {
                    DeleteNote(actualNoteIndex); // ノートを削除
                    break; // 一つ削除したらループを抜ける
                }
            }
        }
    }


    if (input_->IsKeyPressed(DIK_LCONTROL) && input_->IsKeyTriggered(DIK_DOWN))
    {
        // CTRL + DOWNで選択中のノートを下に移動
        scrollOffset_ = 0;
        editorCoordinate_.SetScrollOffset(scrollOffset_);
    }

    if (input_->IsKeyTriggered(DIK_ESCAPE) && mouseInsideEditorArea)
    {
        // ESCキーで選択をクリア
        ClearSelection();
    }
}

void BeatMapEditor::UpdateEditorState(float _deltaTime)
{
    if (isPlaying_)
    {
        currentTime_ += _deltaTime * playSpeed_;

        editorCoordinate_.SetScrollOffset(currentTime_);
    }

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

int32_t BeatMapEditor::GetNoteIndexFromHoldEnd(uint32_t _laneIndex, float _targetTime) const
{
    for (size_t i = 0; i < currentBeatMapData_.notes.size(); ++i)
    {
        const NoteData& note = currentBeatMapData_.notes[i];
        if (note.laneIndex == _laneIndex &&
            note.noteType == "hold" &&
            std::abs(note.targetTime + note.holdDuration - _targetTime) <= 0.05f)
        {
            return static_cast<int32_t>(i); // ノートのインデックスを返す
        }
    }
    return static_cast<int32_t>(-1); // 見つからなかった場合は無効なインデックスを返す

}

void BeatMapEditor::Reset()
{
    currentBeatMapData_ = BeatMapData(); // 譜面データを初期化
    isModified_ = false; // 変更されていない状態にする
    currentTime_ = 0.0f; // 現在の時間を初期化
    isPlaying_ = false; // 再生状態を初期化
    scrollOffset_ = 0.0f; // スクロールオフセットを初期化
    selectedNoteIndices_.clear(); // 選択中のノートインデックスをクリア
    currentEditorMode_ = EditorMode::Select; // エディターモードをノーマルに設定
    drawNoteIndices_.clear();
    noteIndex_ = 0;
    holdNoteIndex_ = 0;
}

void BeatMapEditor::RestartMusic()
{
    if (musicSoundInstance_)
    {
        StopMusic(); // 既存の音楽を停止
        musicVoiceInstance_ = musicSoundInstance_->Play(volume_); // 音楽を再生
    }
}

void BeatMapEditor::PlayMusic()
{
    if (musicSoundInstance_)
    {
        StopMusic(); // 既存の音楽を停止
        musicVoiceInstance_ = musicSoundInstance_->Play(volume_, currentTime_); // 現在の時間から音楽を再生
    }
}

void BeatMapEditor::StopMusic()
{
    if (musicVoiceInstance_)
    {
        musicVoiceInstance_->Stop(); // 音楽を停止
        musicVoiceInstance_ = nullptr; // 音楽のインスタンスをリセット
    }
}
