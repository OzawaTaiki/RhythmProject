#pragma once

#include <Features/UI/UISprite.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>


#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/BeatsManager/BeatManager.h>
#include <Application/Command/CommandHistory.h>

#include <Application/BeatMapEditor/LiveMapping/LiveMapping.h>
#include <Application/BeatMapEditor/BPMCounter/TapBPMCounter.h>

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
    void Update();
    void Draw(const Camera* _camera);
    void Finalize();

public:

    /// <summary>
    /// ノートを配置
    /// </summary>
    /// <param name="_laneIndex">レーン番号</param>
    /// <param name="_targetTime">目標時間</param>
    /// <param name="_noteType">ノートタイプ</param>
    /// <param name="_holdDuration">ホールド時間(ロングノートの場合)</param>
    /// <returns>配置したノートのインデックス</returns>
    size_t PlaceNote(uint32_t _laneIndex, float _targetTime, const std::string& _noteType, float _holdDuration = 0);


    /// <summary>
    /// ノートを削除
    /// </summary>
    /// <param name="_noteIndex">削除するノートのインデックス</param>
    NoteData DeleteNote(size_t _noteIndex);


    /// <summary>
    /// ノートを挿入
    /// </summary>
    /// <param name="_note"> 挿入するノートデータ</param>
    void InsertNote(const NoteData& _note);

    /// <summary>
    /// ノートを取得
    /// </summary>
    /// <param name="_noteIndex"> 取得するノートのインデックス</param>
    /// <returns> ノートデータ</returns>
    const NoteData& GetNoteAt(size_t _noteIndex) const;

    /// <summary>
    /// ノートの時間を設定
    /// </summary>
    /// <param name="_noteIndex"> ノートのインデックス</param>
    /// <param name="_newTime"> 新しい時間</param>
    void SetNoteTime(size_t _noteIndex, float _newTime);


    /// <summary>
    /// ノートを時間でソート
    /// </summary>
    void SortNotesByTime();

    /// <summary>
    /// パラメータからノートを検索
    /// </summary>
    /// <param name="_laneIndex"> レーンインデックス</param>
    /// <param name="_targetTime"> ターゲット時間</param>
    /// <param name="_tolerance"> 許容誤差</param>
    /// <returns> 見つかったノートのインデックス(なければ -1)</returns>
    int32_t FindNoteAtTime(uint32_t _laneIndex, float _targetTime, float _tolerance = 0.05f) const;

    /// <summary>
    /// ロングノートの持続時間を設定
    /// </summary>
    /// <param name="_noteIndex"> ノートのインデックス</param>
    /// <param name="_newDuration"> 新しい持続時間</param>
    void SetNoteDuration(size_t _noteIndex, float _newDuration);
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
    void CreateNewBeatMap(const std::string& _filePath, const std::string& _audioFilePath);


    ///---------------------
    /// 譜面編集



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

    /// <summary>
    /// ライブマッピングを適用
    /// </summary>
    void ApplyLiveMapping();

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
    void UpdateEditorState();



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
    /// 範囲選択エリアのチェック
    /// </summary>
    void CheckSelectionArea(); // 範囲選択エリアのチェック

    /// <summary>
    /// 状態のリセット
    /// </summary>
    void Reset();

    // ========================================
    // 音楽・再生制御
    // ========================================

    /// <summary>
    /// 音楽を始めから再生
    /// </summary>
    void RestartMusic();

    /// <summary>
    /// 音楽を再生
    /// </summary>
    void PlayMusic();

    /// <summary>
    /// 音楽を停止
    /// </summary>
    void StopMusic();

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

    /// <summary>
    /// 配置プレビューのノートを描画
    /// </summary>
    void DrawPreviewNote();

    /// <summary>
    /// 範囲選択エリアを描画
    /// </summary>
    void DrawSelectionArea();

private:
    enum class EditorMode
    {
        Select,
        PlaceNormalNote,
        PlaceLongNote,
        Delete,
        LiveMapping,
        BPMSetting,

        Count // モードの数
    };

private:
    // ========================================
    // システム依存関係
    // ========================================
    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    Camera for2dCamera_;
    BeatMapLoader* beatMapLoader_ = nullptr;
    std::unique_ptr<BeatManager> beatManager_ = nullptr;
    bool enableBeats_ = false;

    // ========================================
    // コア機能
    // ========================================
    CommandHistory commandHistory_;
    EditorCoordinate editorCoordinate_;

    // ========================================
    // 譜面データ
    // ========================================
    BeatMapData currentBeatMapData_;
    std::string currentFilePath_;
    bool isModified_ = false;

    // ========================================
    // 音楽・再生制御
    // ========================================
    std::shared_ptr<SoundInstance> musicSoundInstance_;
    std::shared_ptr<VoiceInstance> musicVoiceInstance_;
    std::shared_ptr<VoiceInstance> voiceInstanceForBPMSet_;
    float volume_ = 0.3f;
    float currentTime_ = 0.0f;
    bool isPlaying_ = false;

    // ========================================
    // エディター状態・設定
    // ========================================
    EditorMode currentEditorMode_ = EditorMode::Select;
    EditorMode preCurrentEditorMode_ = EditorMode::Select;
    bool gridSnapEnabled_ = true;
    float snapInterval_ = 1.0f / 4.0f;

    // ========================================
    // 選択・編集状態
    // ========================================
    std::vector<size_t> selectedNoteIndices_;
    bool isMovingSelectedNote_ = false;
    int32_t lastSelectedNoteIndex_ = -1;
    bool isRangeSelected_ = false;

    // ========================================
    // ロングノート編集状態
    // ========================================
    bool isCreatingLongNote_ = false;
    float longNoteStartTime_ = 0.0f;
    int32_t longNoteStartLane_ = 0;
    bool isSelectingHoldEnd_ = false;
    int32_t selectNoteIndex_ = -1;

    // ========================================
    // 範囲選択・ドラッグ
    // ========================================
    bool isDragging_ = false;
    Vector2 dragStartPosition_ = { 0.0f, 0.0f };
    Vector2 dragEndPosition_ = { 0.0f, 0.0f };

    // ========================================
    // 描画制御・インデックス管理
    // ========================================
    float scrollOffset_ = 0.0f;
    uint32_t noteIndex_ = 0;
    uint32_t holdNoteIndex_ = 0;
    std::vector<uint32_t> drawNoteIndices_;

    // ========================================
    // ノート色設定
    // ========================================
    struct NoteColor {
        Vector4 defaultColor;
        Vector4 hoverColor;
        Vector4 selectedColor;
    };
    NoteColor normalNoteColor_;
    NoteColor longNoteColor_;

    // ========================================
    // プレビュー機能
    // ========================================
    std::unique_ptr<UISprite> previewNoteSprite_;
    std::unique_ptr<UISprite> previewBridgeSprite_;
    std::unique_ptr<UISprite> previewHoldEndSprite_;
    float previewAlpha_ = 0.5f;

    // ========================================
    // 描画用スプライト群
    // ========================================
    // ノート描画
    std::vector<std::unique_ptr<UISprite>> noteSprites_;
    std::vector<std::unique_ptr<UISprite>> noteBridges_;
    std::vector<std::unique_ptr<UISprite>> holdNoteEnd_;

    // レーン・UI描画
    std::vector<std::unique_ptr<UISprite>> laneSprites_;
    std::unique_ptr<UISprite> judgeLineSprite_;
    std::unique_ptr<UISprite> playheadSprite_;

    // 選択・判定用
    std::unique_ptr<UISprite> areaSelectionSprite_;
    std::unique_ptr<UISprite> dummy_editArea_;
    std::unique_ptr<UISprite> dummy_window_;

    // ========================================
    // 特殊機能
    // ========================================
    LiveMapping liveMapping_;
    TapBPMCounter tapBPMCounter_;

};


/*

操作
    1~ モード選択 (選択 ノーマル配置 ロング配置

    削除は選択からDIK_del
    BPM設定とライブマッピングは何か別のキーを

    タイムラインのスクロールはマウスホイールで
    ズームはalt+スクロール

    画面左にはモードとかを
    画面右にノートのパラメータ表示


グリッドの色
    1/1 白
    1/2 緑
    1/4 青
    1/8 赤
    1/16 紫
*/