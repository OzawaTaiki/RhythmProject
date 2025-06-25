#pragma once

#include <Application/Note/Note.h>
#include <Application/BeatMapLoader/BeatMapData.h>

#include <memory>
#include <list>

class Camera;

// 単一のレーン
class Lane
{
public:

    Lane() = default;
    ~Lane() = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="_noteDataList">ノーツのデータ</param>
    /// <param name="_laneIndex">レーンインデックス</param>
    /// <param name="_judgeLine">判定ラインの座標</param>
    void Initialize(const std::list<NoteData>& _noteDataList, int32_t _laneIndex, float _judgeLine);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="_elapseTime">経過時間</param>
    void Update(float _elapseTime);

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="_camera">カメラ</param>
    void Draw(const Camera* _camera) const;


    /// <summary>
    /// 一番手前のノーツを取得
    /// </summary>
    /// <returns>一番手前のノーツへのポインタ</returns>
    Note* GetFirstNote() const;



public: // 静的メンバ関数

    static void SetLaneWidth(float width) { laneWidth_ = width; }
    static float GetLaneWidth() { return laneWidth_; }

    static void SetLaneLength(float length) { laneLength_ = length; }
    static float GetLaneLength() { return laneLength_; }

    static void SetLaneCount(int32_t count) { laneCount_ = count; }
    static int32_t GetLaneCount() { return laneCount_; }

private: // 内部処理用関数たち

    /// <summary>
    /// ノーツを生成する
    /// </summary>
    void CreateNotes(const std::list<NoteData>& _noteDataList, int32_t _laneIndex, float _judgeLine);


private:

    std::list<std::unique_ptr<Note>> notes_; // レーンにあるノーツ

    Vector3 startPosition_; // レーンの開始位置
    Vector3 endPosition_; // レーンの終了位置


private:
    static float laneWidth_; // レーンの幅
    static float laneLength_; // レーンの長さ
    static float totalWidth_; // レーンの合計幅
    static int32_t laneCount_; // レーンの数

};