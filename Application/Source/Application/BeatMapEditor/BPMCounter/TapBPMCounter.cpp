#include "TapBPMCounter.h"

#include <Debug/ImGuiManager.h>

void TapBPMCounter::Initialize()
{
    input_ = Input::GetInstance(); // 入力管理クラスのインスタンスを取得

    stopwatch_.Reset();

    tapTimes_.clear(); // タップ時間のベクターを初期化

    totalTime_ = 0.0f;
}

void TapBPMCounter::Update()
{
    stopwatch_.Update();
    if (input_->IsKeyTriggered(DIK_SPACE)) // スペースキーが押されたら
    {
        float currentTime = stopwatch_.GetElapsedTime<float>(); // 現在の時間を取得

        if (tapTimes_.empty())
        {
            stopwatch_.Start();
        }/*
        else if (tapTimes_.back() != 0.0f && tapTimes_.back() * 2.0f < currentTime)
        {
            stopwatch_.Reset();
            currentTime = 0.0f;
        }*/
        totalTime_ += currentTime;
        if (currentTime != 0.0f)
            tapTimes_.push_back(currentTime); // タップ時間を記録
        stopwatch_.Reset(); // ストップウォッチをリセット
    }

    if (tapTimes_.size() > 10) // タップ時間が100を超えたら古いものを削除
    {
        totalTime_ -= tapTimes_.front(); // 古いタップ時間を合計から引く
        tapTimes_.pop_front(); // 古いタップ時間を削除
    }

#ifdef _DEBUG

    ImGui::Begin("Tap BPM Counter");
    {
        ImGui::Text("BPM : %.1f", GetBPM());
        for (size_t i = 0; i < tapTimes_.size(); ++i)
        {
            ImGui::Text("Tap %zu: %6.2f ms", i + 1, tapTimes_[i] * 1000); // タップ時間を表示
        }
    }
    ImGui::End();

#endif // _DEBUG


}

void TapBPMCounter::Reset()
{
    stopwatch_.Reset(); // ストップウォッチをリセット
    tapTimes_.clear(); // タップ時間のベクターをクリア
    totalTime_ = 0.0f;
}

float TapBPMCounter::GetBPM() const
{
    if (tapTimes_.size() < 2)
        return 0.0f; // タップが2回未満の場合はBPMを計算できない

    int32_t tapCount = static_cast<int32_t>(tapTimes_.size());

    float averageTime = totalTime_ / (tapCount); // タップ間の平均時間を計算

    if (averageTime <= 0.0f)
        return 0.0f; // 平均時間が0以下の場合はBPMを計算できない

    float bpm = 60.0f / averageTime; // BPMを計算

    return bpm; // 計算したBPMを返す

}
