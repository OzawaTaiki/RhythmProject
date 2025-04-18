#include "Lane.h"

#include <Debug/ImGuiDebugManager.h>

Lane::Lane()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("Lane", [&]() { ImGui(); });
#endif // _DEBUG
}

Lane::~Lane()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("Lane");
#endif // _DEBUG
}

void Lane::Initialize()
{
    lineDrawer_ = LineDrawer::GetInstance();

    dirty_ = true;

    // Jsonからデータを取得
    InitializeJsonBinder();

    // レーンの計算
    CalculateLane();
}

void Lane::Update()
{
}

void Lane::Draw()
{
    for (uint32_t i = 1; i < lanePoints_.size() + 1; i += 2)
    {
        lineDrawer_->RegisterPoint(lanePoints_[i - 1], lanePoints_[i], color_);
    }

    DrawCenterLine();
}

Vector3 Lane::GetLaneStartPosition(uint32_t _index) const
{
    if (!isDirty_)
        return laneStartPoints_[_index];

    if (_index >= laneStartPoints_.size())
    {
        throw std::out_of_range("Indexが配列サイズより大きいです。");
    }

    return GeneLaneStartPoints()[_index];
}

void Lane::InitializeJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("Lane" , "Resources/Data/Game/Lane/");

    jsonBinder_->RegisterVariable("LaneCount", &laneCount_);
    jsonBinder_->RegisterVariable("TotalWidth", &totalWidth_);
    jsonBinder_->RegisterVariable("Width", &width_);
    jsonBinder_->RegisterVariable("Length", &length_);
    jsonBinder_->RegisterVariable("Center", &center_);
}

void Lane::CalculateLane()
{
    if(!dirty_)
        return;

    if (laneCount_ <= 1)
    {
        throw std::runtime_error("LaneCountが少ないです 2以上に設定してください");
        dirty_ = false;
        return;
    }

    lanePoints_.clear();

    width_ = totalWidth_ / static_cast<float>(laneCount_);

    // 端の点を計算
    // 手前
    Vector3 leftLineStart = { 0,0,0 };
    leftLineStart.x = center_.x - totalWidth_ / 2.0f;
    leftLineStart.y = center_.y;
    leftLineStart.z = center_.z - length_ / 2.0f;

    // 奥
    Vector3 leftLineEnd = leftLineStart;
    leftLineEnd.z += length_;

    lanePoints_.push_back(leftLineStart);
    lanePoints_.push_back(leftLineEnd);

    // 左のラインを基準に
    for (uint32_t i = 0; i < laneCount_; ++i)
    {
        Vector3 lanePoint = leftLineStart;
        lanePoint.x += width_ * (i + 1);
        lanePoints_.push_back(lanePoint);

        lanePoint.z += length_;
        lanePoints_.push_back(lanePoint);
    }
    GeneLaneStartPoints();
}

std::vector<Vector3> Lane::GeneLaneStartPoints() const
{
    if(isDirty_)
        return laneStartPoints_;

    std::vector<Vector3> laneStartPoints;

    if(lanePoints_.empty())
    {
        throw std::runtime_error("LanePointsが空です");
        return laneStartPoints;
    }


    for (uint32_t i = 0; i < lanePoints_.size() / 2 - 1; ++i)
    {
        Vector3 spoint = lanePoints_[i * 2 + 1 ];
        spoint.x += width_ / 2.0f;

        laneStartPoints.push_back(spoint);
    }

    laneStartPoints_ = laneStartPoints;

    return laneStartPoints;
}

void Lane::DrawCenterLine()
{
    if (laneStartPoints_.empty())
        GeneLaneStartPoints();

    for (size_t index = 0; index < laneStartPoints_.size(); ++index)
    {
        Vector3 start = laneStartPoints_[index];
        Vector3 end = start;
        end.z -= length_;
        lineDrawer_->RegisterPoint(start, end, { 1,0,0,1 });
    }
}


void Lane::ImGui()
{

#ifdef _DEBUG

    ImGui::PushID(this);


    if (ImGui::InputInt("LaneCount", reinterpret_cast<int*>(&laneCount_), 1))
        dirty_ = true;

    if (ImGui::DragFloat("TotalWidth", &totalWidth_, 0.01f))
        dirty_ = true;

    if (ImGui::DragFloat("Width", &width_, 0.01f))
        dirty_ = true;

    if (ImGui::DragFloat("Length", &length_, 0.01f))
        dirty_ = true;

    if (ImGui::DragFloat3("Center", &center_.x, 0.01f))
        dirty_ = true;

    if(ImGui::Button("Save"))
        jsonBinder_->Save();

    ImGui::PopID();

    CalculateLane();

#endif // _DEBUG

}
