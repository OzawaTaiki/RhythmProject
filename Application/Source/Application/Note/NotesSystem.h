#pragma once

// Engine
#include <System/Time/Stopwatch.h>

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/Note.h>
#include <Application/BeatMapLoader/BeatMapData.h>

// STL
#include <list>
#include <memory>

class NotesSystem
{
public:
    NotesSystem(Lane* _lane);
    ~NotesSystem();

    void Initialize(float _noteSpeed, float _noteSize);
    void Update(float _deltaTime);
    void DrawNotes(const Camera* _camera);

    void SetBeatMapData(const BeatMapData& _beatMapData) { beatMapData_ = _beatMapData; }

    float GetNoteSpeed() const { return noteSpeed_; }

    void SetJudgeLinePosition(float _position) { judgeLinePosition_ = _position; }

    void SetMissJudgeThreshold(float _threshold) { missJudgeThreshold_ = _threshold; }

#ifdef _DEBUG
    void SetStopwatch(Stopwatch* _stopwatch) { stopwatch_ = _stopwatch; }
#endif // _DEBUG

private:

    void CreateNormalNote(uint32_t _laneIndex, float _speed, float _targetTime);

    void CreateLongNote(uint32_t _laneIndex, float _speed, float _targetTime,std::shared_ptr<Note> _nextNote);

    void DebugWindow();
private:

    float noteSpeed_ = 0.0f;
    float noteSize_ = 0.0f;

    float judgeLinePosition_ = 0.0f;
    float missJudgeThreshold_ = 0.5f;

    Lane* lane_ = nullptr;
    std::list<std::shared_ptr<Note>> notes_;

    BeatMapData beatMapData_;

#ifdef _DEBUG
    Stopwatch* stopwatch_;
#endif // _DEBUG
};