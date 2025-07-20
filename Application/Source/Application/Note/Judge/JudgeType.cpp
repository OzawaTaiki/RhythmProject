#include "JudgeType.h"

std::string Judge::ToString(JudgeType _type)
{
    switch (_type)
    {
    case JudgeType::None:       return "None";
    case JudgeType::Perfect:    return "Perfect";
    case JudgeType::Good:       return "Good";
    case JudgeType::Miss:       return "Miss";
    default:                    return "Unknown";
    }
}