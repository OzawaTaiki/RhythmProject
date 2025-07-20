#pragma once

#include <string>

enum class JudgeType
{
    None = 0,

    Perfect,
    Good,
    Bad,
    Miss,

    MAX
};
namespace Judge
{
    std::string ToString(JudgeType _type);
}
