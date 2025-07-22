#include "GameUI.h"

#include <Debug/ImGuiDebugManager.h>

void GameUI::Initialize()
{
    textGenerator_.Initialize(FontConfig());

    jsonBinder_ = std::make_unique<JsonBinder>("GameUI", "Resources/Data/UI/");

    jsonBinder_->RegisterVariable("ComboValueTextParam", &comboValueParam_);
    jsonBinder_->RegisterVariable("ComboTextParam", &comboTextParam_);

}

void GameUI::Update(int32_t _combo)
{
    comboValue_ = _combo;
    ImGui();
}

void GameUI::Draw()
{
    textGenerator_.Draw(std::format(L"{}", comboValue_), comboValueParam_);
    textGenerator_.Draw(L"Combo", comboTextParam_);
}

void GameUI::ImGui()
{
#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("GameUI"))
    {
        ImGui::SeparatorText("value");
        ImGui::PushID("val");
        comboValueParam_.ImGui();
        ImGui::PopID();


        ImGui::SeparatorText("text");
        ImGui::PushID("tex");
        comboTextParam_.ImGui();
        ImGui::PopID();

        if(ImGui::Button("Save"))
        {
            jsonBinder_->Save();
        }

        ImGui::End();
    }
#endif // _DEBUG
}
