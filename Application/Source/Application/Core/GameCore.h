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
    void Initialize(const BeatMapData& _beatMapData);

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


    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> _voiceInstance) { musicVoiceInstance_ = _voiceInstance; }

private:

    void ParseBeatMapData(const BeatMapData& _beatMapData);

private:

    // note
    // lane
    std::vector<std::unique_ptr<Lane>> lanes_; // レーンのリスト
    // judge

    // score あとまわし

    // note spawner あったらいいな

    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス 弱参照

};