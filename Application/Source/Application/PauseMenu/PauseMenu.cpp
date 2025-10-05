#include "PauseMenu.h"

#include <System/Input/Input.h>
#include <Debug/ImGuiDebugManager.h>


#include <Features/UI/UISprite.h>
#include <Features/UI/UIButton.h>
#include <Features/UI/UISlider.h>
#include <Debug/Debug.h>
#include <Application/Setting/Setting.h>

#include <System/Audio/AudioSystem.h>



void PauseMenu::Initialize()
{

    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    auto backSprite = uiGroup_->CreateSprite("PauseMenu_blackback", L"Pause_blackback");
    backSprite->SetSize({ 1280, 720 });
    backSprite->SetColor({ 0, 0, 0, 0.8f }); // 半透明の黒背景
    backSprite->SetPos({ 0, 0 });
    backSprite->SetAnchor({ 0,0 });// 左上にアンカーを設定

    auto sprite = uiGroup_->CreateSprite("PauseMenu_back", L"Pause");

    auto resumeButton = uiGroup_->CreateButton("PauseMenu_ResumeButton", L"Resume");
    //resumeButton->SetParent(sprite);

    auto retryButton = uiGroup_->CreateButton("PauseMenu_RetryButton",L"Retry");
    //retryButton->SetParent(sprite);

    auto toTitleButton = uiGroup_->CreateButton("PauseMenu_ToTitleButton", L"Title");
    //toTitleButton->SetParent(sprite);

    sprite->AddChild(resumeButton);
    sprite->AddChild(retryButton);
    sprite->AddChild(toTitleButton);

    UIGroup::LinkHorizontal(
        { resumeButton.get(), retryButton.get(), toTitleButton.get() }
    );

    //sprite_= sprite;


    buttons_["PauseMenu_ResumeButton"] = resumeButton;
    buttons_["PauseMenu_RetryButton"] = retryButton;
    buttons_["PauseMenu_ToTitleButton"] = toTitleButton;
}

void PauseMenu::Update()
{
    if (Input::GetInstance()->IsKeyTriggered(DIK_ESCAPE))
    {
        isActive_ = true;
        actions_ =  PauseActions::Open; // ポーズメニューの開閉をトグル

    }

    if (!isActive_)
        return;

    uiGroup_->Update();

    if(settingMenu_)
    {
        settingMenu_->Update();
    }

#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("pauseMenu"))
    {
        if (ImGui::CollapsingHeader("sprite"))
            sprite_->ImGui();


        for (auto& [key, button] : buttons_)
        {
            if (ImGui::CollapsingHeader(key.c_str()))
            {
                button->ImGui();
            }
        }

        ImGui::End();
    }


#endif

}

void PauseMenu::Draw()
{
    if (!isActive_)
        return;

    uiGroup_->Draw();

    if (settingMenu_)
    {
        settingMenu_->Draw();
    }
}

void PauseMenu::SetCallBacks(const std::function<void()>& _onResumeCallback, const std::function<void()>& _onRetryCallback, const std::function<void()>& _onToTitleCallback)
{
    SetOnResumeCallback(_onResumeCallback);
    SetOnRetryCallback(_onRetryCallback);
    SetOnToTitleCallback(_onToTitleCallback);
}

void PauseMenu::SetOnResumeCallback(const std::function<void()>& _callback)
{
    if (_callback)
    {
        onResumeCallback_ = _callback;

        auto it = buttons_.find("PauseMenu_ResumeButton");
        if (it != buttons_.end())
        {
            it->second->SetOnClickEnd([this]() {
                isActive_ = false; // ポーズメニューを閉じる
                actions_ = PauseActions::Resume; // 選択されたボタンを更新
                onResumeCallback_(); // コールバックを呼び出す
                });
        }
    }
}

void PauseMenu::SetOnRetryCallback(const std::function<void()>& _callback)
{
    if (_callback)
    {
        onRetryCallback_ = _callback;
        auto it = buttons_.find("PauseMenu_RetryButton");
        if (it != buttons_.end())
        {
            it->second->SetOnClickEnd([this]() {
                isActive_ = false; // ポーズメニューを閉じる
                actions_ = PauseActions::Retry; // 選択されたボタンを更新
                onRetryCallback_(); // コールバックを呼び出す
                });
        }
    }
}

void PauseMenu::SetOnToTitleCallback(const std::function<void()>& _callback)
{
    if (_callback)
    {
        onToTitleCallback_ = _callback;
        auto it = buttons_.find("PauseMenu_ToTitleButton");
        if (it != buttons_.end())
        {
            it->second->SetOnClickEnd([this]() {
                isActive_ = false; // ポーズメニューを閉じる
                actions_ = PauseActions::ToTitle; // 選択されたボタンを更新
                onToTitleCallback_(); // コールバックを呼び出す
                });
        }
    }
}
