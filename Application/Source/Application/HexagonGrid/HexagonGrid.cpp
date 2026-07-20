#include "HexagonGrid.h"
#include <Features/UI/Collider/UIConvexPolygonCollider.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/UI/Component/UIColliderComponent.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>
#include <numbers>

using namespace Engine;

namespace
{
// 描画エリア
Rect drawArea(Vector2(0, 0), Vector2(1280, 720));
}

void HexagonGrid::Initialize(const Rect& area)
{
    moveOffset_ = Vector2(0.0f, 0.0f); // オフセットを初期化

    Vector2 halfSize = area.size;
    // 描画エリアを拡張
    Vector2 expandedMin = area.GetLeftTop() - halfSize;
    Vector2 expandedMax = area.GetRightBottom() + halfSize;
    GenerateHexagonGrid(Rect(expandedMin, expandedMax));

}

void HexagonGrid::Update()
{
    ImGui();
    // 六角形の幅と高さを計算
    float width = radius_ * std::sqrtf(3.0f);
    float height = radius_ * 1.5f;

    // グリッド全体のオフセットを更新
    moveOffset_ += Vector2(-2.0f, 1.0f) * 0.016f;

    // ループ処理: オフセットが一定値を超えたらリセット
    // X方向のループ（2列分で1周期）
    if (moveOffset_.x < -width * 2.0f)
    {
        moveOffset_.x += width * 2.0f;
    }
    else if (moveOffset_.x > width * 2.0f)
    {
        moveOffset_.x -= width * 2.0f;
    }

    // Y方向のループ（2行分で1周期）
    if (moveOffset_.y < -height * 2.0f)
    {
        moveOffset_.y += height * 2.0f;
    }
    else if (moveOffset_.y > height * 2.0f)
    {
        moveOffset_.y -= height * 2.0f;
    }

    // 各六角形の位置を更新（初期位置 + オフセット）

    for (size_t i = 0; i < elements_.size() && i < initialPositions_.size(); ++i)
    {
        elements_[i]->SetPosition(initialPositions_[i] + moveOffset_);
        elements_[i]->Update();
    }

}

void HexagonGrid::Draw()
{
    for (const auto& element : elements_)
    {
        element->Draw();
    }
}

void HexagonGrid::GetHexagonLocalVertices(std::vector<Vector2>& outVertices) const
{
    float angleStep = std::numbers::pi_v<float> *2.0f / 6.0f;
    float drawRadius = radius_ - margin_; // marginを引いた半径で描画

    outVertices.clear();
    for (int i = 0; i < 6; ++i)
    {
        float angle = angleStep * i - std::numbers::pi_v<float> / 6.0f;
        float x = drawRadius * std::cosf(angle);
        float y = drawRadius * std::sinf(angle);
        outVertices.emplace_back(x, y);
    }
}

void HexagonGrid::GenerateHexagonGrid(const Rect& area)
{
    std::vector<Vector2> hexagonVertices;
    GetHexagonLocalVertices(hexagonVertices);

    // 六角形の幅と高さを計算
    // マジックナンバー √3 と 1.5 は正六角形の幾何学的特性に基づく
    // らしい
    constexpr float kHexWidthRatio = std::numbers::sqrt3_v<float>;
    constexpr float kHexHeightRatio = 1.5f;

    float width = radius_ * kHexWidthRatio;
    float height = radius_ * kHexHeightRatio;
    // 逶ｴ蠕・
    float diameter = (radius_ - margin_) * 2.0f;

    Vector2 start = area.GetLeftTop();
    Vector2 end   = area.GetRightBottom();
    // marginは配置計算に含めない
    int32_t colSize = static_cast<int32_t>(std::ceilf((end.x - start.x) / width)) + 2;
    int32_t rowSize = static_cast<int32_t>(std::ceilf((end.y - start.y) / height)) + 2;

    initialPositions_.clear();

    for (int32_t row = 0; row < rowSize; ++row)
    {
        for (int32_t col = 0; col < colSize; ++col)
        {
            Vector2 center = GetHexagonCenter(row, col);
            center += start; // エリアの開始位置を加算

            initialPositions_.push_back(center); // 初期位置を保存

            auto collider = std::make_unique<UIConvexPolygonCollider>();
            collider->SetTransformMode(IUICollider::TransformMode::Independent);
            collider->SetLocalVertices(hexagonVertices);

            int32_t num = col + row * colSize; // インデックス計算も修正
            std::string label = "hexagon_";
            if (num < 10)
                label += "0";

            auto hexagon = std::make_unique<UIButtonElement>(label + std::to_string(num), center, Vector2( diameter, diameter ), "", false);
            hexagon->Initialize();
            hexagon->SetPosition(center);
            hexagon->SetAnchor({ 0.5f,0.5f });
            hexagon->SetSize({ diameter , diameter });
            hexagon->GetComponent<UIColliderComponent>()->SetCollider(std::move(collider));
            hexagon->SetHoverColor({ 0.15f, 0.26f, 0.3f, 0.5f });
            hexagon->SetNormalColor({ 0.125f, 0.125f, 0.19f, 0.5f });
            hexagon->GetComponent<UISpriteRenderComponent>()->LoadAndSetTexture("Hexagon.png");

            elements_.push_back(std::move(hexagon));
            // Todo: イベント設定 ↓
            // カラーは変化なし
            // ホバー中はビートで拡縮など
            // 全体として常に
            // 荳螳壽婿蜷代∈豬√☆
            // パーティクルを舞わせる
            // とか

        }
    }
}

Vector2 HexagonGrid::GetHexagonCenter(int32_t row, int32_t col) const
{
    float width  = radius_ * std::sqrtf(3.0f);
    float height = radius_ * 1.5f;

    Vector2 center;
    center.x = col * width;
    center.y = row * height;

    // 奇数行をx方向にオフセット
    if (row % 2 == 1)
    {
        center.x += width * 0.5f;
    }

    return center;
}

void HexagonGrid::ImGui()
{
#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("HexagonGrid Settings"))
    {
        ImGui::DragFloat("Radius", &radius_, 1.0f, 10.0f, 500.0f);
        ImGui::DragFloat("Margin", &margin_, 0.5f, 0.0f, radius_ - 1.0f);
        if (ImGui::Button("Regenerate Grid"))
        {
            // 蜀咲函謌・
            Initialize(drawArea);
        }

        ImGui::End();
    }

#endif // _DEBUG
}