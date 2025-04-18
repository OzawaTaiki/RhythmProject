#include "NoteJudge.h"

#include <Features/Event/EventManager.h>

NoteJudge::NoteJudge()
{
    EventManager::GetInstance()->AddEventListener("noteJudge", this);
}

NoteJudge::~NoteJudge()
{
    EventManager::GetInstance()->RemoveEventListener("noteJudge", this);
}

void NoteJudge::Initialize()
{
}
