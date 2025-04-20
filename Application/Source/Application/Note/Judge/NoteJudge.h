#pragma once

// Engine
#include <Features/Json/JsonBinder.h>
#include <Features/Event/EventListener.h>

// Application
#include <Application/EventData/NoteJudgeType.h>

// STL
#include <memory>



class NoteJudge : public iEventListener
{
public:
    NoteJudge();
    ~NoteJudge();

    void Initialize();

    void DrawJudgeLine();

    void OnEvent(const GameEvent& _event) override;


    void SetPosition(float _position) { position_ = _position; }
    void SetLaneTotalWidth(float _width) { laneTotalWidth_ = _width; }
    void SetSpeed(float _speed) { speed_ = _speed; }

    float GetMissJudgeThreshold() { return timingThresholds_[NoteJudgeType::Miss]; }
private:

    void InitializeJsonBinder();

private:

    std::unique_ptr<JsonBinder> jsonBinder_;

    // 判定 閾値
    std::map<NoteJudgeType, float> timingThresholds_;

    /// debug用
    float position_ = 0.0f;
    float laneTotalWidth_ = 0.0f;
    float speed_ = 0.0f;

};
