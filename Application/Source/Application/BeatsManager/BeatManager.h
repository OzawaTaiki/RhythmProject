#pragma once

#include <System/Time/Stopwatch.h>
#include <string>
#include <cstdint>

class BeatManager
{
public:
    BeatManager();
    ~BeatManager() = default;

    // 初期化 - BPMとオフセットを設定
    void Initialize(float bpm, float offset = 0.0f, const std::string& soundPath = "Resources/Sounds/Metronome.wav");

    // 更新
    void Update();

    // 拍動作の開始
    void Start();

    // 拍動作の停止
    void Stop();

    // リセット
    void Reset();

    // 現在の拍数を取得（小数部分も含む）
    float GetCurrentBeat() const;

    // 最寄りの整数拍数を取得
    int GetNearestBeat() const;

    // 新しい拍かどうかをチェック
    bool IsNewBeat();

    // 拍が発生したかどうかをチェック（特定のタイミング範囲内）
    bool IsBeatTriggered(float tolerance = 0.05f) const;

    // BPM変更
    void SetBPM(float bpm);

    // ボリューム設定
    void SetVolume(float volume) { volume_ = volume; }

    // サウンド有効・無効設定
    void EnableSound(bool enable) { soundEnabled_ = enable; }

private:
    Stopwatch stopwatch_;
    float bpm_ = 120.0f;       // 1分あたりの拍数
    float offset_ = 0.0f;      // 開始オフセット（秒）
    int lastBeat_ = -1;        // 最後に処理した拍数
    bool playing_ = false;     // 再生中かどうか

    // サウンド関連
    uint32_t soundHandle_ = 0;
    uint32_t voiceHandle_ = 0;
    float volume_ = 0.5f;
    bool soundEnabled_ = true;

    // 1拍あたりの秒数を計算
    float GetSecondsPerBeat() const { return 60.0f / bpm_; }
};