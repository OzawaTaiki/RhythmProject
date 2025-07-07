#include "EditorCoordinate.h"
#include <Debug/Debug.h>

EditorCoordinate::EditorCoordinate() :
    screenSize_(1280.0f, 720.0f),
    areaCenter_(640.0f, 360.0f), // 初期エリアセンター
    laneCount_(4),
    editAreaX_(100.0f),
    editAreaWidth_(400.0f),
    laneWidth_(100.0f),
    laneMargin_(0.0f),
    zoom_(1.0f),
    scrollOffset_(0.0f),
    pixelsPerSecond_(1000.0f),  // 1秒=100ピクセル（基準値
    cachedVisibleStartTime_(0.0f),
    cachedVisibleEndTime_(0.0f),
    visibleRangeDirty_(true),
    timeZeroOffsetRatio_(0.2f),
    topMargin_(50.0f), // 上部マージン
    bottomMargin_(50.0f) // 下部マージン
{
}

void EditorCoordinate::Initialize(const Vector2& _screenSize, const Vector2& _areaCenter, uint32_t _laneCount)
{
    screenSize_ = _screenSize;
    laneCount_ = _laneCount;

    areaCenter_ = _areaCenter;

    UpdateLayout();
    InvalidateVisibleRange();
}

void EditorCoordinate::SetScreenSize(const Vector2& _screenSize)
{
    SetScreenSize(_screenSize.x, _screenSize.y);
}

void EditorCoordinate::SetScreenSize(float _width, float _height)
{
    screenSize_.x = _width;
    screenSize_.y = _height;

    UpdateLayout();
    InvalidateVisibleRange();
}

float EditorCoordinate::GetLaneLeftX(uint32_t _laneIndex) const
{
    if (_laneIndex >= laneCount_) {
        return 0.0f;  // 無効なレーン
    }
    // レーンの左端X座標を返す
    float laneX = editAreaX_ + (_laneIndex * (laneWidth_ + laneMargin_));
    return laneX;
}

float EditorCoordinate::GetLaneRightX(uint32_t _laneIndex) const
{
    if (_laneIndex >= laneCount_) {
        return 0.0f;  // 無効なレーン
    }
    // レーンの右端X座標を返す
    return GetLaneLeftX(_laneIndex) + laneWidth_;
}

float EditorCoordinate::TimeToScreenY(float _time) const
{
    // 下がtime=0
    // スクロールオフセットを考慮
    float adjustedTime = _time - scrollOffset_;
    // offsetを考慮
    float effectiveHeight = screenSize_.y - topMargin_ - bottomMargin_;
    float baseY = bottomMargin_ + effectiveHeight *  (1.0f - timeZeroOffsetRatio_);

    return baseY - (adjustedTime * GetPixelsPerSecond());
}

float EditorCoordinate::ScreenYToTime(float _screenY) const
{
    // Y座標から時間を逆算
    float effectiveHeight = screenSize_.y - topMargin_ - bottomMargin_;
    float baseY = bottomMargin_ + effectiveHeight * (1.0f - timeZeroOffsetRatio_);

    float adjustedTime = (baseY - _screenY) / GetPixelsPerSecond();
    return adjustedTime + scrollOffset_;
}

float EditorCoordinate::LaneToScreenX(uint32_t _laneIndex) const
{
    if (_laneIndex >= laneCount_) {
        return 0.0f;  // 無効なレーン
    }

    // レーンの中央X座標を返す
    float laneX = editAreaX_ + (_laneIndex * (laneWidth_ + laneMargin_));
    return laneX + (laneWidth_ * 0.5f);
}

int32_t EditorCoordinate::ScreenXToLane(float _screenX) const
{
    // 編集エリア外なら無効
    if (_screenX < editAreaX_ || _screenX > editAreaX_ + editAreaWidth_) {
        return -1;
    }

    // どのレーンに含まれるか計算
    float relativeX = _screenX - editAreaX_;
    int32_t laneIndex = static_cast<int>(relativeX / (laneWidth_ + laneMargin_));

    // レーンの境界内にあるかチェック
    if (laneIndex >= 0 && laneIndex < static_cast<int>(laneCount_)) {
        float laneStartX = laneIndex * (laneWidth_ + laneMargin_);
        float laneEndX = laneStartX + laneWidth_;

        if (relativeX >= laneStartX && relativeX <= laneEndX) {
            return laneIndex;
        }
    }

    return -1;  // レーン境界外
}

void EditorCoordinate::SetZoom(float _zoom)
{
    zoom_ = std::clamp(_zoom, 0.1f, 1000.0f); // ズーム倍率を制限
    InvalidateVisibleRange();
}

void EditorCoordinate::SetScrollOffset(float _offset)
{
    scrollOffset_ = _offset;
    InvalidateVisibleRange();
}

void EditorCoordinate::GetVisibleTimeRange(float& _startTime, float& _endTime) const
{
    if (visibleRangeDirty_) {

        cachedVisibleStartTime_ = ScreenYToTime(screenSize_.y - bottomMargin_);
        cachedVisibleEndTime_ = ScreenYToTime(topMargin_); // 上端は0.0f

        //Debug::Log(std::format("Visible Time Range: Start = {:.2f}, End = {:.2f}\n", cachedVisibleStartTime_, cachedVisibleEndTime_));
        //Debug::Log(std::format("startPosY = {:.2f}, endPosY = {:.2f}\n", TimeToScreenY(cachedVisibleStartTime_), TimeToScreenY(cachedVisibleEndTime_)));

        visibleRangeDirty_ = false; // 可視範囲が更新されたのでフラグをリセット
    }

    _startTime = cachedVisibleStartTime_;
    _endTime = cachedVisibleEndTime_;
}

void EditorCoordinate::SetTopMargin(float _margin)
{
    topMargin_ = std::max(_margin, 0.0f); // マージンは0以上に制限
    InvalidateVisibleRange();
}

void EditorCoordinate::SetBottomMargin(float _margin)
{
    bottomMargin_ = std::max(_margin, 0.0f); // マージンは0以上に制限
    InvalidateVisibleRange();
}

void EditorCoordinate::SetVerticalMargins(float _topMargin, float _bottomMargin)
{
    topMargin_ = std::max(_topMargin, 0.0f); // 上部マージンは0以上に制限
    bottomMargin_ = std::max(_bottomMargin, 0.0f); // 下部マージンは0以上に制限
    InvalidateVisibleRange();
}

void EditorCoordinate::SetVerticalMargins(const Vector2& _margin)
{
    SetVerticalMargins(_margin.x, _margin.y);
}

std::vector<std::pair<float, int32_t>>  EditorCoordinate::GetGridLinesY(float _bpm, int _division) const
{
    static const std::vector<int32_t> divisions = { 1, 2, 4, 8, 16 }; // 分割数の候補

    // グリッドラインのY座標と1/n拍のインデックスをペアで返す
    std::vector<std::pair<float, int32_t>>  gridLines;

    if (_bpm <= 0 || _division <= 0)
    {
        // 無効なBPMまたは分割数
        return gridLines;
    }

    float beatInterval = 60.0f / _bpm; // 1拍の時間（秒）

    float gridInterval = beatInterval / _division; // グリッド1つの時間（秒）

    float start, end;
    GetVisibleTimeRange(start, end);

    start -= offsetTime_; // オフセット時間を引く
    end -= offsetTime_; // オフセット時間を引く

    int32_t startIndex = static_cast<int32_t>(std::floorf(start / gridInterval)); // 開始グリッドのインデックス
    int32_t endIndex = static_cast<int32_t>(std::ceilf(end / gridInterval)); // 終了グリッドのインデックス
    const float epsilon = 1.0e-3f; // 浮動小数点の許容誤差

    for (int32_t i = startIndex; i <= endIndex; ++i)
    {
        // 累積誤差の影響を受けないように、グリッド時間を都度計算
        float time = i * gridInterval + offsetTime_; // グリッド時間を計算
        float screenY = TimeToScreenY(time); // Y座標に変換
        if (screenY >= topMargin_ && screenY <= screenSize_.y - bottomMargin_) // 画面内に収まるかチェック
        {
            for (int32_t j = 0; j < divisions.size(); ++j)
            {
                int32_t d = divisions[j];
                float gridUnit = beatInterval / static_cast<float>(d); // 1/n拍の時間（秒）
                float val = (time - offsetTime_) / gridUnit; // グリッド単位での時間
                if (std::abs(val - std::roundf(val)) < 1.0e-3f) // 誤差を考慮して分割数が一致するかチェック
                {
                    gridLines.push_back({ screenY, j }); // Y座標と分割数をペアで保存
                    break; // 一度見つけたらループを抜ける
                }
            }
        }
    }

    //累積誤差の影響でうまく動作しない
    // // 可視範囲の開始時間から終了時間までのグリッドラインを計算
    //float firstGridTime = std::floorf(start / gridInterval) * gridInterval; // 最初のグリッド時間を計算
    //
    //const float epsilon = 1.0e-3f; // 浮動小数点の誤差を考慮
    //for (float time = firstGridTime; time <= end; time += gridInterval)
    //{
    //    float screenY = TimeToScreenY(time);
    //    if (screenY >= topMargin_ && screenY <= screenSize_.y - bottomMargin_ ) // 画面内に収まるかチェック
    //    {
    //        for (int32_t i = 0; i < divisions.size(); ++i)
    //        {
    //            // 誤差のせいで描画されない
    //            int32_t d = divisions[i];
    //            float gridUnit = beatInterval / static_cast<float>(d); // 1/n拍の時間（秒）
    //            float val = time / gridUnit; // グリッド単位での時間
    //            // 分割数が一致するかチェック
    //            if (std::abs(val - std::roundf(val)) < epsilon)
    //            {
    //                gridLines.push_back({ screenY, i }); // Y座標と分割数をペアで保存
    //                break; // 一度見つけたらループを抜ける
    //            }
    //        }
    //    }
    //}

    return gridLines;
}

float EditorCoordinate::SnapTimeToGrid(float _time, float _bpm, int _division) const
{
    if (_bpm <= 0 || _division <= 0)
    {
        return _time;
    }

    float beatInterval = 60.0f / _bpm;
    float gridInterval = beatInterval / _division;

    // 楽曲時間 → 音楽的時間に変換
    float musicTime = _time - offsetTime_;

    // 音楽的時間でグリッドにスナップ
    float snappedMusicTime = std::roundf(musicTime / gridInterval) * gridInterval;

    // 音楽的時間 → 楽曲時間に戻す
    return snappedMusicTime + offsetTime_;
}

bool EditorCoordinate::IsNoteVisible(float _noteTime) const
{
    float startTime, endTime;
    GetVisibleTimeRange(startTime, endTime);

    return _noteTime >= startTime && _noteTime <= endTime;
}

void EditorCoordinate::SetTimeZeroOffsetRatio(float _ratio)
{
    // 0.0f ~ 1.0fの範囲で設定
    timeZeroOffsetRatio_ = std::clamp(_ratio, 0.0f, 1.0f);

}

void EditorCoordinate::UpdateLayout()
{
    // 編集エリアのレイアウト計算
    // 画面の中央部分をエディット領域として使用
    float sideMargin = screenSize_.x * 0.1f;  // 左右10%をマージン
    editAreaX_ = areaCenter_.x - screenSize_.x / 2.0f + sideMargin;
    editAreaWidth_ = screenSize_.x - (sideMargin * 2.0f) ;

    // レーン幅計算（4レーン等分）
    laneMargin_ = editAreaWidth_ * 0.02f;  // レーン間の余白（全体の2%）
    float totalLaneMargin = laneMargin_ * (laneCount_ - 1);
    laneWidth_ = (editAreaWidth_ - totalLaneMargin) / laneCount_;
}

void EditorCoordinate::InvalidateVisibleRange()
{
    visibleRangeDirty_ = true;
}
