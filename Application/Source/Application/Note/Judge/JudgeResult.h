#pragma once

// Engine

// Application
#include <Application/Note/Judge/JudgeType.h>

// STL
#include <map>
#include <cstdint>

// 判定をもっておくクラス
class JudgeResult
{
public:
    JudgeResult();
    ~JudgeResult();

    void Initialize();


    /// <summary>
    /// 判定結果を反映する
    /// </summary>
    void AddJudge(JudgeType _judgeType);

    /// <summary>
    /// 判定結果を反映する（カウントを指定できる）
    /// </summary>
    /// <param name="_judgeType"> 判定タイプ</param>
    /// <param name="_count"> カウント</param>
    void AddJudge(JudgeType _judgeType, int32_t _count);

    /// <summary>
    /// 判定結果を取得する
    /// </summary>
    /// <returns>判定結果のマップ</returns>
    const std::map<JudgeType, uint32_t>& GetJudgeResult() const { return judgeResult_; }

    /// <summary>
    /// 指定した判定タイプの結果を取得する
    /// </summary>
    /// <param name="_judgeType">判定タイプ</param>
    /// <returns>判定結果の数</returns>
    uint32_t GetJudgeResult(JudgeType _judgeType) const;

    void DebugWindow();
private:

    std::map<JudgeType, uint32_t> judgeResult_; // 判定結果



};