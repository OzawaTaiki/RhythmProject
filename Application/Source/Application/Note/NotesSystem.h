#pragma once

// Engine
#include <System/Time/Stopwatch.h>

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/Note.h>
#include <Application/BeatMapLoader/BeatMapData.h>

#include <System/Audio/SoundInstance.h>

// STL
#include <list>
#include <memory>

class NotesSystem
{
public:
    NotesSystem(Lane* _lane);
    ~NotesSystem();

    void Initialize(float _noteSpeed, float _noteSize, float _startOffsetTime = 2.0f);
    void Update(float _deltaTime);
    void DrawNotes(const Camera* _camera);

    void SetBeatMapDataAndCreateNotes(const BeatMapData& _beatMapData);

    float GetNoteSpeed() const { return noteSpeed_; }

    void SetJudgeLinePosition(float _position) { judgeLinePosition_ = _position; }

    void SetMissJudgeThreshold(float _threshold) { missJudgeThreshold_ = _threshold; }

    void SetStopwatch(Stopwatch* _stopwatch) { stopwatch_ = _stopwatch; }

    void Reload();

    void playing(bool _playing) { playing_ = _playing; }

    void SetAutoPlay(bool _autoPlay) { autoPlay_ = _autoPlay; }

    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> _voiceInstance) { musicVoiceInstance_ = _voiceInstance; }

    bool IsReloaded();

    void Start() { isStarted_ = true; }

private:

    void CreateNormalNote(uint32_t _laneIndex, float _speed, float _targetTime);

    void CreateLongNote(const NoteData& _noteData);
    std::shared_ptr<Note> CreateNextNoteForLongNote(uint32_t _laneIndex, float _speed, float _targetTime);

    void DebugWindow();
private:

    float noteSpeed_ = 0.0f;
    float noteSize_ = 0.0f;

    float judgeLinePosition_ = 0.0f;
    // miss判定で消えるまでの猶予座標
    float missJudgeThreshold_ = 3.0f;

    bool isStarted_ = false; // ゲーム開始オフセット時間を超えたかどうか
    float waitTimer_ = 0.0f; // ゲーム開始オフセット時間を超えるまでのタイマ
    float startOffsetTime_ = 0.0f; // ゲーム開始オフセット時間

    Lane* lane_ = nullptr;
    std::list<std::shared_ptr<Note>> notes_;

    bool autoPlay_ = false;

    BeatMapData beatMapData_;

    bool playing_ = true;

    Stopwatch* stopwatch_;

    bool isReloaded_ = false;

    // 再生している音楽データ
    std::shared_ptr<VoiceInstance> musicVoiceInstance_ = nullptr;

};
