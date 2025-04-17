#pragma once

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Json/JsonBinder.h>

// Application


// STL
#include <memory>

class Lane
{
public:
    Lane();
    ~Lane();

    void Initialize();
    void Update();
    void Draw();

    Vector3 GetLaneStartPosition(uint32_t _index) const;

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


    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr;

    bool dirty_ = false;

};

