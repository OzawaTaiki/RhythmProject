#pragma once

#include <Framework/Framework.h>

class SampleFramework : public Framework
{
public:
    SampleFramework() = default;
    ~SampleFramework() = default;

    void Initialize(const std::wstring& _winTitle) override;
    void Update() override;
    void Draw() override;

    void Finalize() override;

private:

};
