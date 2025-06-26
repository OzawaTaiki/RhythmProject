#pragma once

// Engine
#include <Features/Json/JsonBinder.h>

// Application
#include <Application/Note/Judge/JudgeType.h>

// STL
#include <memory>

class Note;

class NoteJudge
{
public:
    NoteJudge();
    ~NoteJudge();

    void Initialize();

    void DrawJudgeLine();

    /// <summary>
    /// ノーツの判定を行う
    /// </summary>
    /// <param name="_note">判定するノート</param>
    /// <param name="elapsedTime">経過時間</param>
    JudgeType ProcessNoteJudge(Note* _note, float _elapsedTime);

    void SetIsDrawLine(bool _isDraw) { isDrawLine = _isDraw; }

    void SetPosition(float _position) { position_ = _position; }
    void SetLaneTotalWidth(float _width) { laneTotalWidth_ = _width; }
    void SetSpeed(float _speed) { speed_ = _speed; }

    float GetGoodJudgeThreshold() { return timingThresholds_[JudgeType::Good]; }
    float GetMissJudgeThreshold() { return timingThresholds_[JudgeType::Miss]; }
private:

    void InitializeJsonBinder();

private:

    std::unique_ptr<JsonBinder> jsonBinder_;

    // 判定 閾値
    std::map<JudgeType, float> timingThresholds_;

    /// debug用
    float position_ = 0.0f;
    float laneTotalWidth_ = 0.0f;
    float speed_ = 0.0f;

    bool isDrawLine = true;

};
