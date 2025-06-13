#pragma once

#include <Math/Vector/Vector2.h>

#include <cstdint>

/// <summary>
/// えぢたー用座標変換システム
/// </summary>
class EditorCoordinate
{
public:
    EditorCoordinate();
    ~EditorCoordinate() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="_screenSize">画面サイズ</param>
    /// <param name="_laneCount">レーン数(デフォルト4)</param>
    void Initialize(const Vector2& _screenSize, uint32_t _laneCount = 4);

    /// <summary>
    /// 画面サイズを設定
    /// </summary>
    /// <param name="_screenSize">画面サイズ</param>
    void SetScreenSize(const Vector2& _screenSize);

    /// <summary>
    /// 画面サイズを設定
    /// </summary>
    /// <param name="_width">画面幅</param>
    /// <param name="_height">画面高さ</param>
    void SetScreenSize(float _width, float _height);

    // ========================================
    // 座標変換
    // ========================================

    /// <summary>
    /// 時間を画面Y座標に変換
    /// </summary>
    /// <param name="_time">時間(s)</param>
    /// <returns>スクリーンY座標</returns>
    float TimeToScreenY(float _time) const;

    /// <summary>
    /// 時間を画面Y座標に変換
    /// </summary>
    /// <param name="_screenY">画面Y座標</param>
    /// <returns>時間(s)</returns>
    float ScreenYToTime(float _screenY) const;

    /// <summary>
    /// レーンインデックスから画面X座標に変換
    /// </summary>
    /// <param name="_laneIndex"> レーンインデックス</param>
    /// <returns> スクリーンX座標</returns>
    float LaneToScreenX(uint32_t _laneIndex) const;

    /// <summary>
    /// 画面X座標からレーンインデックスに変換
    /// </summary>
    /// <param name="_screenX">画面X座標</param>
    /// <returns>レーンインデックス</returns>
    int32_t ScreenXToLane(float _screenX) const;


    // ========================================
    // ビューポート制御
    // ========================================

    /// <summary>
    /// ズーム倍率を設定
    /// </summary>
    /// <param name="_zoom"> ズーム倍率</param>
    void SetZoom(float _zoom);

    /// <summary>
    /// スクロールオフセットを設定
    /// </summary>
    /// <param name="_offset">スクロールオフセット</param>
    void SetScrollOffset(float _offset);

    /// <summary>
    /// 表示中の時間範囲取得
    /// </summary>
    /// <param name="_startTime"> 開始時間</param>
    /// <param name="_endTime"> 終了時間</param>
    void GetVisibleTimeRange(float& _startTime,float& _endTime) const;



    // ========================================
    // グリッドライン
    // ========================================

    /// <summary>
    /// グリッドラインのY座標を取得
    /// </summary>
    /// <param name="_bpm"楽曲BPM</param>
    /// <param name="_division">分割数（4=1/4拍、8=1/8拍など）</param>
    /// <returns>グリッドラインのY座標リスト</returns>
    std::vector<float> GetGridLinesY(float _bpm, int _division = 4) const;


    /// <summary>
    /// 時間をグリッドにスナップ
    /// </summary>
    /// <param name="_time"> 時間(s)</param>
    /// <param name="_bpm"> 楽曲BPM</param>
    /// <param name="_division"> 分割数（4=1/4拍、8=1/8拍など）</param>
    /// <returns> スナップされた時間(s)</returns>
    float SnapTimeToGrid(float _time, float _bpm, int _division = 4) const;


    /// <summary>
    /// ノートが画面内にあるかチェック
    /// </summary>
    /// <param name="_notetime"> ノートの時間(s)</param>
    /// <returns>画面内にある場合true</returns>
    bool IsNoteVisible(float _noteTime) const;


    // ========================================
    // 設定取得
    // ========================================

    float GetZoom() const { return zoom_; }
    float GetScrollOffset() const { return scrollOffset_; }
    float GetLaneWidth() const { return laneWidth_; }
    uint32_t GetLaneCount() const { return laneCount_; }
    float GetPixelsPerSecond() const { return pixelsPerSecond_ * zoom_; }


    // 内部計算用
    void UpdateLayout();
    void InvalidateVisibleRange();
private:

    Vector2 screenSize_; // 画面サイズ
    uint32_t laneCount_; // レーン数

    // レイアウト設定
    float editAreaX_;        // 編集エリアの左端X座標
    float editAreaWidth_;    // 編集エリアの幅
    float laneWidth_;        // 1レーンの幅
    float laneMargin_;       // レーン間の余白

    float zoom_ ; // ズーム倍率
    float scrollOffset_; // スクロールオフセット
    float pixelsPerSecond_; // 秒あたりのピクセル数


    // 表示範囲キャッシュ
    mutable float cachedVisibleStartTime_;
    mutable float cachedVisibleEndTime_;
    mutable bool visibleRangeDirty_;
};