#pragma once

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Math/Vector/Vector4.h>

// Application


// STL


class JudgeLine
{
public:
    JudgeLine();
    ~JudgeLine();

    void Initialize();
    void Draw();


    float GetPosition() const { return position_; }

private:

    void CalculateLine();

    void DebugWindow();

private:

    float position_ = -0.0f;

    Vector4 color_ = { 0,1,1,1 };

    LineDrawer* lineDrawer_ = nullptr;

    std::vector<Vector3> linePoints_;
    bool dirty_ = true;

};