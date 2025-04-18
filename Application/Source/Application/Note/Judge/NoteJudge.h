#pragma once

// Engine
#include <Features/Json/JsonBinder.h>
#include <Features/Event/EventListener.h>

// Application


// STL
#include <memory>

class NoteJudge : public iEventListener
{
public:
    NoteJudge();
    ~NoteJudge();

    void Initialize();


private:

    std::unique_ptr<JsonBinder> jsonBinder_;

};
