#pragma once

// Engine

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/Note.h>

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

private:

    void CreateNote(uint32_t _laneIndex, float _speed);

    void DebugWindow();
private:

    float noteSpeed_ = 0.0f;
    float noteSize_ = 0.0f;

    Lane* lane_ = nullptr;
    std::list<std::shared_ptr<Note>> notes_;

};