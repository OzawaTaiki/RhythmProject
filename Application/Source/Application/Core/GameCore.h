#pragma once

#include <System/Audio/VoiceInstance.h>

#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/Lane/Lane.h>

class Camera;

// ゲームの核となる部分

class GameCore
{
public:

    // コンストラクタ
    GameCore(int32_t _laneCount = 4);
    // デストラクタ
    ~GameCore();


    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(float _noteSpeed, float _offset = 2.0f);

    /// <summary>
    /// 更新処理
    /// </summary>
    // 後々/ <param name="_inputData">入力データ</param>
    void Update(float  _deltaTime);

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="_camera">カメラ</param>
    void Draw(const Camera* _camera);

    /// <summary>
    /// 譜面データを受け取り、ノーツを生成する
    /// </summary>
    /// <param name="_beatMapData">譜面データ</param>
    void GenerateNotes(const BeatMapData& _beatMapData);

    /// <summary>
    /// 音楽の音声インスタンスを設定する
    /// </summary>
    /// <param name="_voiceInstance">音声インスタンスを</param>
    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> _voiceInstance) { musicVoiceInstance_ = _voiceInstance; }


    /// <summary>
    /// 開始
    /// </summary>
    void Start() { isWaitingForStart_ = false; }
private:

    void ParseBeatMapData(const BeatMapData& _beatMapData);

private:

    // note
    // lane
    float noteSpeed_ = 30.0f; // ノーツの移動速度
    int32_t laneCount_ = 4; // レーンの数
    std::vector<std::unique_ptr<Lane>> lanes_; // レーンのリスト
    // judge

    // score あとまわし

    // note spawner あったらいいな



    /// 開始前オフセット関連
    float offset_ = 2.0f; // ゲーム開始オフセット時間
    float waitTimer_ = 0.0f; // 開始前オフセット待機タイマー
    bool isWaitingForStart_ = true; // 開始前オフセット待機中かどうか


    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス 弱参照

};