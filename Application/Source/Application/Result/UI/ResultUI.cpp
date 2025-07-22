#include "ResultUI.h"

#include <Debug/ImguITools.h>
#include <Debug/ImGuiDebugManager.h>


void ResultUI::Initialize(ResultData _resultData)
{
    musicTitle_ = _resultData.musicTitle;

    textParams_[TextType::Score_value].counterValue = std::make_optional<CounterValue>();
    textParams_[TextType::Score_value].counterValue->value = _resultData.score;

    textParams_[TextType::Combo_value].counterValue = std::make_optional<CounterValue>();
    textParams_[TextType::Combo_value].counterValue->value = _resultData.combo;

    for (auto& [judgeType, count] : _resultData.judgeResult)
    {
        TextType textType = GetTextTypeFromJudgeType(judgeType);
        textParams_[textType].counterValue = std::make_optional<CounterValue>();
        textParams_[textType].counterValue->value = count;
    }

#ifdef _DEBUG
    for (int32_t i = 0; i < static_cast<int32_t>(JudgeType::MAX);++i)
    {
        TextType textType = GetTextTypeFromJudgeType(static_cast<JudgeType>(i));
        auto& param = textParams_[textType];
        if (!param.counterValue.has_value())
            param.counterValue = std::make_optional<CounterValue>();

        if (param.counterValue->value == -1)
            param.counterValue->value = 192;
    }
#endif // _DEBUG


    animationSequence_ = std::make_unique<AnimationSequence>("ResultUISeq");
    animationSequence_->Initialize("Resources/Data/Result/");
    animationSequence_->SetLooping(false);

    InitUIGroup();
    InitTextParams();

    text_.Initialize(FontConfig());
}


void ResultUI::Update(float _deltaTime)
{
    static bool seqUpdate = false;

#ifdef _DEBUG

    if(ImGuiDebugManager::GetInstance()->Begin("ResultUI Debug"))
    {
        if (ImGui::Button("Save"))
            jsonBinder_->Save();

        ImGui::Checkbox("Animation Sequence Update", &seqUpdate);
        if(ImGui::Button("Reset"))
        {
            animationSequence_->SetCurrentTime(0.0f);
            for (auto& [type, textParam] : textParams_)
                textParam.animationValue.timer = -textParam.animationValue.delay;
        }

        ImGuiTool::TimeLine("ResultUI Animation", animationSequence_.get());

        ImGui::Separator();
        if (ImGui::Button("seq Save"))
            animationSequence_->Save();
        ImGui::Separator();

        ImGui::DragFloat("Duration", &animationDuration_, 0.01f, 0.01f, 10.0f);

        for (int32_t i = 0; i < static_cast<int32_t>(TextType::Count); ++i)
        {
            TextType textType = static_cast<TextType>(i);
            auto& param = textParams_[textType];
            if (ImGui::CollapsingHeader(GetKeyString(textType).c_str()))
            {
                ImGui::DragFloat2("Position", &param.textParam.position.x);
                ImGui::DragFloat2("Scale", &param.textParam.scale.x, 0.01f);
                ImGui::DragFloat("Rotate", &param.textParam.rotate, 0.01f);
                ImGui::DragFloat2("Pivot", &param.textParam.pivot.x, 0.01f);
                ImGui::Checkbox("Use Gradient", &param.textParam.useGradient);
                ImGui::Checkbox("Use Outline", &param.textParam.useOutline);
                ImGui::ColorEdit4("Bottom Color", &param.textParam.bottomColor.x);
                ImGui::ColorEdit4("Top Color", &param.textParam.topColor.x);
                ImGui::ColorEdit4("Outline Color", &param.textParam.outlineColor.x);
                ImGui::DragFloat("Outline Scale", &param.textParam.outlineScale, 0.01f);
                ImGui::DragFloat("Delay", &param.animationValue.delay, 0.01f);
            }
        }

        for (auto& sprite : debugSprites_)
        {
            if (sprite)
            {
                sprite->ImGui();
            }
        }
        for (auto& button : debugButtons_)
        {
            if (button)
            {
                button->ImGui();
            }
        }

        ImGui::End();
    }

#endif // _DEBUG


    // アニメーションの更新
    if (animationSequence_)
    {
        float duration = animationSequence_->GetMaxPlayTime();

        for (auto& [type,textParam] : textParams_)
        {
            auto& animParam = textParam.animationValue;
            animParam.timer += _deltaTime;
            if (animParam.timer >= duration)
            {
                animParam.timer = duration;
            }
            if (animParam.timer < 0)
            {
                animationSequence_->SetCurrentTime(0);

            }
            else
            {
                animationSequence_->SetCurrentTime(animParam.timer);
            }
            animationSequence_->Update(0.0f);
            animParam.movement = animationSequence_->GetValue<Vector2>("movement");
            animParam.scale = animationSequence_->GetValue<Vector2>("scale");
            animParam.alpha = animationSequence_->GetValue<float>("alpha");

            textParam.textParam.position = animParam.position + animParam.movement;
            textParam.textParam.scale = animParam.scale;
            textParam.textParam.topColor.w = animParam.alpha;
            textParam.textParam.bottomColor.w = animParam.alpha;

            if (animParam.timer >= 0 && textParam.counterValue.has_value())
            {
                auto& counterValue = textParam.counterValue.value();

                counterValue.animationTimer += _deltaTime;
                if (counterValue.animationTimer >= animationDuration_)
                {
                    counterValue.animationTimer = animationDuration_;
                }
                // アニメーションの更新
                float progress = counterValue.animationTimer / animationDuration_;
                counterValue.currentValue = static_cast<int32_t>(counterValue.value * progress);
                textParam.label = std::format(L"{}", counterValue.currentValue);
            }

        }
        uiGroup_->Update();
    }



}

void ResultUI::Draw()
{
    Sprite::PreDraw();
    uiGroup_->Draw();

    for (const auto& [textType, param] : textParams_)
    {
        if (textType == TextType::Count)
            break;

        // テキストの描画
        text_.Draw(param.label, param.textParam);
    }
}

void ResultUI::InitUIGroup()
{
    uiGroup_ = std::make_unique<UIGroup>();
    uiGroup_->Initialize();

    auto mainBg = uiGroup_->CreateSprite("main_bg");

    TextParam param;
    param.SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    auto toTitleButton = uiGroup_->CreateButton("To_Title",L"タイトルへ");

    toTitleButton->SetCallBackOnClickEnd([this]()
        {
            transitionToTitle_ = true;
        });
    toTitleButton->SetTextParam(param);

    auto retryButton = uiGroup_->CreateButton("Retry", L"リトライ");

    retryButton->SetCallBackOnClickEnd([this]()
        {
            replay_ = true;
        });
    retryButton->SetTextParam(param);

    UIGroup::LinkHorizontal({ toTitleButton, retryButton });

#ifdef _DEBUG
    // デバッグ用のスプライトとボタンを追加
    debugSprites_.push_back(mainBg);
    debugButtons_.push_back(toTitleButton);
    debugButtons_.push_back(retryButton);
#endif // _DEBUG

}

void ResultUI::InitTextParams()
{
    jsonBinder_ = std::make_unique<JsonBinder>("ResutUIs","Resources/Data/Result/");

    jsonBinder_->RegisterVariable("animationDuration", &animationDuration_);

    for (int32_t i = 0; i < static_cast<int32_t>(TextType::Count); ++i)
    {
        TextType textType = static_cast<TextType>(i);
        auto& param = textParams_[textType];

        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_pos", &param.textParam.position);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_scale", &param.textParam.scale);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_rotate", &param.textParam.rotate);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_pivot", &param.textParam.pivot);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_useGradient", &param.textParam.useGradient);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_useOutline", &param.textParam.useOutline);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_bottomColor", &param.textParam.bottomColor);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_topColor", &param.textParam.topColor);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_outlineColor", &param.textParam.outlineColor);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_outlineScale", &param.textParam.outlineScale);
        jsonBinder_->RegisterVariable(GetKeyString(textType) + "_delay", &param.animationValue.delay);

        param.label = GetTextLabel(textType);

        param.animationValue.position = param.textParam.position; // アニメーション用の初期位置
        param.animationValue.scale = param.textParam.scale; // アニメーション用の初期スケール
        param.animationValue.alpha = param.textParam.topColor.w; // アニメーション用の初期アルファ値
        param.animationValue.timer = -param.animationValue.delay; // アニメーション用の初期タイマー

    }

}

std::string ResultUI::GetKeyString(TextType _textType) const
{
    switch (_textType)
    {
    case TextType::Title:
        return "Title";
    case TextType::Score_text:
        return "Score_Text";
    case TextType::Score_value:
        return "Score_Value";
    case TextType::Judge_perfect_text:
        return "Perfect_Text";
    case TextType::Judge_perfect_value:
        return "Perfect_count";
    case TextType::Judge_good_text:
        return "Good_Text";
    case TextType::Judge_good_value:
        return "Good_count";
    case TextType::Judge_bad_text:
        return "Bad_Text";
    case TextType::Judge_bad_value:
        return "Bad_count";
    case TextType::Judge_miss_text:
        return "Miss_Text";
    case TextType::Judge_miss_value:
        return "Miss_count";
    case TextType::Combo_text:
        return "Combo_Text";
    case TextType::Combo_value:
        return "Combo_count";
    default:
        return "";
    }
}

std::wstring ResultUI::GetTextLabel(TextType _textType) const
{
    switch (_textType)
    {
    case TextType::Title:
        return L"None";
    case TextType::Score_text:
        return L"Score";
    case TextType::Score_value:
        return L"0";
    case TextType::Judge_perfect_text:
        return L"Perfect";
    case TextType::Judge_perfect_value:
        return L"0";
    case TextType::Judge_good_text:
        return L"Good";
    case TextType::Judge_good_value:
        return L"0";
    case TextType::Judge_bad_text:
        return L"Bad";
    case TextType::Judge_bad_value:
        return L"0";
    case TextType::Judge_miss_text:
        return L"Miss";
    case TextType::Judge_miss_value:
        return L"0";
    case TextType::Combo_text:
        return L"Combo";
    case TextType::Combo_value:
        return L"0";
    default:
        return L"";
    }
}

JudgeType ResultUI::GetJudgeTypeFromTextType(TextType _textType) const
{
    switch (_textType)
    {
    case TextType::Judge_perfect_value:
        return JudgeType::Perfect;
    case TextType::Judge_good_value:
        return JudgeType::Good;
    case TextType::Judge_bad_value:
        return JudgeType::Bad;
    case TextType::Judge_miss_value:
        return JudgeType::Miss;
    default:
        return JudgeType::None;
    }
}

ResultUI::TextType ResultUI::GetTextTypeFromJudgeType(JudgeType _judgeType) const
{
    switch (_judgeType)
    {
    case JudgeType::Perfect:
        return TextType::Judge_perfect_value;
    case JudgeType::Good:
        return TextType::Judge_good_value;
    case JudgeType::Bad:
        return TextType::Judge_bad_value;
    case JudgeType::Miss:
        return TextType::Judge_miss_value;
    case JudgeType::MAX:
    case JudgeType::None:
    default:
        return TextType::Count;
    }
}
