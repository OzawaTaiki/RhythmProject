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

    void SetBeatMapDataAndCreateNotes(const BeatMapData& _beatMapData);

    float GetNoteSpeed() const { return noteSpeed_; }

    void SetJudgeLinePosition(float _position) { judgeLinePosition_ = _position; }

    void SetMissJudgeThreshold(float _threshold) { missJudgeThreshold_ = _threshold; }

    void SetStopwatch(Stopwatch* _stopwatch) { stopwatch_ = _stopwatch; }

    void Reload();

private:

    void CreateNormalNote(uint32_t _laneIndex, float _speed, float _targetTime);

    void CreateLongNote(uint32_t _laneIndex, float _speed, float _targetTime,std::shared_ptr<Note> _beforeNote);
    void CreateLongNote(const NoteData& _noteData);
    std::shared_ptr<Note> CreateNextNoteForLongNote(uint32_t _laneIndex, float _speed, float _targetTime);

    void DebugWindow();
private:

    float noteSpeed_ = 0.0f;
    float noteSize_ = 0.0f;

    float judgeLinePosition_ = 0.0f;
    // miss判定で消えるまでの猶予座標
    float missJudgeThreshold_ = 3.0f;

    Lane* lane_ = nullptr;
    std::list<std::shared_ptr<Note>> notes_;

    BeatMapData beatMapData_;

    bool playing_ = true;

    Stopwatch* stopwatch_;
};