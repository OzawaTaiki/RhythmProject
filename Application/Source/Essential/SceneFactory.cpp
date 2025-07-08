#include "SceneFactory.h"

#include "SampleScene.h"
#include <Features/Scene/ParticleTestScene.h>

#include <Application/Scene/GameScene.h>
#include <Application/Scene/TitleScene.h>
#include <Application/Scene/SelectScene.h>
#include <Application/Scene/ResultScene.h>
#include <Application/Scene/EditorScene.h>

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& _name)
{
    if (_name == "Sample")
    {
        return std::make_unique<SampleScene>();
    }
    else if (_name == "ParticleTest")
    {
        //return std::make_unique<ParticleTestScene>();
    }
    else if (_name == "GameScene")
    {
        return std::make_unique<GameScene>();
    }
    else if (_name == "TitleScene")
    {
        return std::make_unique<TitleScene>();
    }
    else if (_name == "SelectScene")
    {
        return std::make_unique<SelectScene>();
    }
    else if (_name == "ResultScene")
    {
        return std::make_unique<ResultScene>();
    }
    else if (_name == "EditorScene")
    {
        return std::make_unique<EditorScene>();
    }



    assert("Scene Not Found");

    return nullptr;
}

std::string SceneFactory::ShowDebugWindow()
{
#ifdef _DEBUG

    ImGui::SeparatorText("Scene Factory");

    if (ImGui::Button("Sample"))
    {
        return "Sample";
    }
   /* if (ImGui::Button("ParticleTest"))
    {
        return "ParticleTest";
    }*/
    if (ImGui::Button("GameScene"))
    {
        return "GameScene";
    }
    if (ImGui::Button("TitleScene"))
    {
        return "TitleScene";
    }
    if (ImGui::Button("SelectScene"))
    {
        return "SelectScene";
    }
    if (ImGui::Button("ResultScene"))
    {
        return "ResultScene";
    }
    if (ImGui::Button("EditorScene"))
    {
        return "EditorScene";
    }


#endif // _DEBUG
    return "";

}
