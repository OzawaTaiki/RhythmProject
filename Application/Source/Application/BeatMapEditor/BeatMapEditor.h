#pragma once

#include <Features/UI/UISprite.h>
#include <Features/Camera/Camera/Camera.h>

#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Application/BeatMapLoader/BeatMapData.h>

#include <string>
#include <cstdint>
#include <vector>
#include <memory>


class Input;
class LineDrawer;

class BeatMapLoader;

class BeatMapEditor
{
public:
    BeatMapEditor() = default;
    ~BeatMapEditor() = default;


    void Initialize();
    void Update(float _deltaTime);
    void Draw(const Camera* _camera);
    void Finalize();

private:
    ///---------------------
    /// ファイル操作

    /// <summary>
    /// 譜面データをロード
    /// </summary>
    /// <param name="_beatMapPath">Jsonファイルパス</param>
    void LoadBeatMap(const std::string& _beatMapPath);

    /// <summary>
    /// 譜面データを保存
    /// </summary>
    /// <param name="_beatMapPath"> 保存するJsonファイルパス</param>
    void SaveBeatMap(const std::string& _beatMapPath);

    /// <summary>
    /// 新しい譜面データを作成
    /// </summary>
    /// <param name="_audioFilePath">作成する音楽ファイルパス</param>
    void CreateNewBeatMap(const std::string& _audioFilePath);


    ///---------------------
    /// 譜面編集

    /// <summary>
    /// ノートを配置
    /// </summary>
    /// <param name="_laneIndex">レーン番号</param>
    /// <param name="_targetTime">は一時間</param>
    /// <param name="_noteType">ノートタイプ</param>
    /// <param name="_holdDuration">ホールド時間(ロングノートの場合)</param>
    void PlaceNote(uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration = 0);


    /// <summary>
    /// ノートを削除
    /// </summary>
    /// <param name="_noteIndex">削除するノートのインデックス</param>
    void DeleteNote(uint32_t _noteIndex);


    /// <summary>
    /// ノートを選択
    /// </summary>
    /// <param name="_noteIndex">選択するノートのインデックス</param>
    /// <param name="_multiSelect">複数選択フラグ</param>
    void SelectNote(uint32_t _noteIndex,bool _multiSelect);

    /// <summary>
    /// 選択をクリア
    /// </summary>
    void ClearSelection();

    /// <summary>
    /// 選択中のノートを移動
    /// </summary>
    /// <param name="_deltaTime">新しい時間</param>
    void MoveSelectedNote(float _newTime);


    ///---------------------
    /// 内部処理関連
    ///

    /// <summary>
    /// 入力処理
    /// </summary>
    void HandleInput();

    /// <summary>
    /// エディターの状態更新
    /// </summary>
    /// <param name="_deltaTime">deltaTime</param>
    void UpdateEditorState(float _deltaTime);

    /// <summary>
    /// パラメータからノートを検索
    /// </summary>
    /// <param name="_laneIndex"> レーンインデックス</param>
    /// <param name="_targetTime"> ターゲット時間</param>
    /// <param name="_tolerance"> 許容誤差</param>
    /// <returns> 見つかったノートのインデックス(なければ -1)</returns>
    int32_t FindNoteAtTime(uint32_t _laneIndex, float _targetTime, float _tolerance = 0.05f) const;

    /// <summary>
    /// ノートを時間でソート
    /// </summary>
    void SortNotesByTime();

    /// <summary>
    /// ノートが選択されているか確認
    /// </summary>
    bool IsNoteSelected(uint32_t _noteIndex) const;

    /// <summary>
    /// ホールド終端から対応するノートインデックスを取得
    /// </summary>
    /// <param name="_laneIndex">レーンインデックス</param>
    /// <param name="_targetTime">ターゲットタイム</param>
    /// <returns></returns>
    int32_t GetNoteIndexFromHoldEnd(uint32_t _laneIndex, float _targetTime) const;


    /// <summary>
    /// 状態のリセット
    /// </summary>
    void Reset();

    // =========================================
    // 描画
    // ==========================================

    /// <summary>
    /// ノートを描画
    /// </summary>
    void DrawNotes();

    /// <summary>
    /// ノートを描画
    /// </summary>
    void DrawNote(const NoteData& _note);

    /// <summary>
    /// レーンを描画
    /// </summary>
    void DrawLanes();

    /// <summary>
    /// グリッドラインを描画
    /// </summary>
    void DrawGridLines();

    /// <summary>
    /// ジャッジラインを描画
    /// </summary>
    void DrawJudgeLine(); // ジャッジラインを描画

    /// <summary>
    /// 再生ヘッドを描画
    /// </summary>
    void DrawPlayhead();// 現在再生している位置のマーカー

    /// <summary>
    /// UIを描画
    /// </summary>
    void DrawUI();


    void DrawPreviewNote();

private:

    LineDrawer* lineDrawer_ = nullptr; // ライン描画クラスへのポインタ
    Input* input_ = nullptr; // 入力管理クラスへのポインタ

    Camera for2dCamera_; // 2D描画用のカメラ

    BeatMapLoader* beatMapLoader_ = nullptr;
    EditorCoordinate editorCoordinate_;

    // 譜面データ
    BeatMapData currentBeatMapData_; // 現在編集中の譜面データ
    std::string currentFilePath_ = "Resources/Data/Game/BeatMap/demo.json"; // 現在編集中のファイルパス
    bool isModified_ = false; // 譜面が変更されたかどうかのフラグ

    // エディター状態
    float currentTime_ = 0.0f; // 現在の時間
    bool isPlaying_ = false; // 再生中かどうかのフラグ
    float playSpeed_ = 1.0f; // 再生速度

    // 選択状態
    std::vector<size_t> selectedNoteIndices_; // 選択中のノートインデックス
    bool gridSnapEnabled_ = true; // グリッドスナップの有効/無効
    float snapInterval_ = 1.0f / 4.0f; //1/4拍

    // =========================================
    // エディター座標系

    float scrollOffset_ = 0.0f; // スクロールオフセット

    uint32_t noteIndex_ = 0; // ノートのインデックスカウンター
    uint32_t holdNoteIndex_ = 0; // ブリッジのインデックスカウンター
    std::vector<std::unique_ptr<UISprite>> noteSprites_; // ノートのスプライトリスト 描画用

    struct NoteColor
    {
        Vector4 defaultColor; // デフォルトのノート色
        Vector4 hoverColor; // ホバー時の色
        Vector4 selectedColor; // 選択時の色
    };

    NoteColor normalNoteColor_;
    NoteColor longNoteColor_; // ロングノートの色設定

    std::vector<uint32_t> drawNoteIndices_; // 描画するノートのインデックスリスト

    bool isSelectingHoldEnd_ = false; // ロングノート終端を選択中かどうかのフラグ
    int32_t selectNoteIndex_ = -1; // 選択中のノートインデックス


    enum class EditorMode
    {
        Select,
        PlaceNormalNote,
        PlaceLongNote,
        Delete,

        Count // モードの数
    };

    EditorMode currentEditorMode_ = EditorMode::Select; // 現在のエディターモード
    bool isCreatingLongNote_ = false; // ロングノートを作成中かどうかのフラグ
    float longNoteStartTime_ = 0.0f; // ロングノートの開始時間
    int32_t longNoteStartLane_ = 0; // ロングノートの開始レーン


    // 配置プレビュー
    std::unique_ptr<UISprite> previewNoteSprite_; // ノート配置プレビュー用のスプライト
    std::unique_ptr<UISprite> previewBridgeSprite_; // ロングノート配置プレビュー用のスプライト
    std::unique_ptr<UISprite> previewHoldEndSprite_; // ロングノート終端配置プレビュー用のスプライト
    float previewAlpha_ = 0.5f; // プレビューの透明度



    //=========================================
    // 描画用

    std::vector<std::unique_ptr<UISprite>> laneSprites_; // レーンのスプライト
    std::vector<std::unique_ptr<UISprite>> gridLineSprites_; // グリッドラインのスプライト

    std::vector<std::unique_ptr<UISprite>> noteBridges_; // ブリッジのスプライト
    std::vector<std::unique_ptr<UISprite>> holdNoteEnd_; // ロングノートの終端スプライト

    std::unique_ptr<UISprite> judgeLineSprite_; // ジャッジラインのスプライト

    std::unique_ptr<UISprite> playheadSprite_; // 再生ヘッドのスプライト


};