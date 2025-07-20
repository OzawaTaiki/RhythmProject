#pragma once

#include <Application/Note/Judge/JudgeType.h>

#include <map>
#include <cstdint>
#include <string>


struct ResultData
{
    // 楽曲のタイトル
    std::string musicTitle = "";

    // 判定結果を保持するマップ
    std::map<JudgeType, uint32_t> judgeResult;

    // コンボ数
    int32_t combo = 0;

    // スコア
    int32_t score = 0;
};