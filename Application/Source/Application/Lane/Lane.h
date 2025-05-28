#pragma once

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Json/JsonBinder.h>
#include <Features/Event/EventListener.h>

// Application
#include <Application/Note/Note.h>
#include <Application/Effects/LaneEffect/LaneEffect.h>


// STL
#include <memory>
#include <list>

class Lane : public iEventListener
{
public:
    Lane();
    ~Lane();

    void Initialize();
    void Update();
    void Draw(const Camera* _camera);

    /// <summary>
    /// レーンのスタート位置を取得
    /// </summary>
    /// <param name="_index">レーンインデックス</param>
    /// <returns>レーンのスタート座標</returns>
    Vector3 GetLaneStartPosition(uint32_t _index) const;


    /// <summary>
    /// レーンにノーツを追加
    /// </summary>
    /// <param name="_index">レーンインデックス</param>
    /// <param name="_note"ノーツのポインタ></param>
    void AddNote(uint32_t _index, std::shared_ptr<Note> _note);

    /// <summary>
    /// レーンの合計幅を取得
    /// </summary>
    /// <returns>レーンの合計幅</returns>
    float GetLaneTotalWidth() const { return totalWidth_; }

    /// <summary>
    /// 判定を取る最大判定を設定
    /// </summary>
    /// <param name="_window">判定を取る最大判定</param>
    void SetJudgeWindow(float _window) { judgeWindow_ = _window; }

    const std::vector<Vector3>& GetLanePoints() const { return lanePoints_; }


    void OnEvent(const GameEvent& _event) override;
private:

    void InitializeJsonBinder();

    void CalculateLane();

    std::vector<Vector3> GeneLaneStartPoints() const;

    void DrawCenterLine();

    void ImGui();
private:

    LineDrawer* lineDrawer_ = nullptr;

    // Laneの数
    uint32_t laneCount_ = 4;

    // Laneの中心 zは手前
    Vector3 center_ = { 0,0,0 };

    // 合計の幅
    float totalWidth_ = 4.0f;
    // 1Laneの幅
    float width_ = 1.0f;
    // Laneの長さ
    float length_ = 10.0f;

    std::vector<Vector3> lanePoints_;

    // Laneの色
    Vector4 color_ = { 1,1,1,1 };

    mutable std::vector<Vector3> laneStartPoints_;

    mutable bool isDirty_ = false;

    // レーンごとのノーツの参照
    std::map<uint32_t, std::list<std::shared_ptr<Note>>> notes_;

    std::vector<std::unique_ptr<LaneEffect>> laneEffects_;

    // 判定を取る最大範囲 -> ミス判定となる時間
    double judgeWindow_ = 0.5f;

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr;

    bool dirty_ = false;

};

