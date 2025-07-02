#pragma once

// Engine
#include <System/Audio/VoiceInstance.h>

// Application
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/Lane/Lane.h>
#include <Application/Input/InputData.h>
#include <Application/Note/Judge/NoteJudge.h>
#include <Application/Note/Judge/JudgeResult.h>

// STL
#include <functional>


// 前方宣言
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
    /// <param name="_deltaTime">デルタタイム</param>
    /// <param name="_inputData">入力データ</param>
    void Update(float  _deltaTime, const std::vector<InputDate>& _inputData);

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

    /// <summary>
    /// リスタート
    /// </summary>
    void Restart(std::shared_ptr<VoiceInstance> _voiceInstance);

    /// <summary>
    /// 判定時のコールバック関数を設定する
    /// </summary>
    /// <param name="_callback">コールバック関数</param>
    void SetJudgeCallback(const std::function<void(int32_t,JudgeType)>& _callback) { onJudgeCallback_ = _callback; }

private:

    void JudgeNotes(const std::vector<InputDate>& _inputData);

    /// <summary>
    /// ノーツを生成する
    /// </summary>
    /// <param name="_beatMapData">譜面データ</param>
    void ParseBeatMapData(const BeatMapData& _beatMapData);

    void CreateBeatMapNotes();

private:

    // note
    // lane
    float noteSpeed_ = 30.0f; // ノーツの移動速度
    int32_t laneCount_ = 4; // レーンの数
    std::vector<std::unique_ptr<Lane>> lanes_; // レーンのリスト
    std::vector<std::list<NoteData>> notesPerLane_; // レーンごとのノートデータリスト

    // judge
    std::unique_ptr<NoteJudge> noteJudge_; // ノーツの判定を行うクラス
    std::unique_ptr<JudgeResult> judgeResult_; // 判定結果を保持するクラス

    // score あとまわし

    // note spawner あったらいいな

    //-------------------------
    // コールバック関連

    // 判定時のコールバック関数
    std::function<void(int32_t, JudgeType)> onJudgeCallback_;


    float noteDeletePosition_ = -10.0f; // ノーツを削除する位置

    /// 開始前オフセット関連
    float offset_ = 2.0f; // ゲーム開始オフセット時間
    float waitTimer_ = 0.0f; // 開始前オフセット待機タイマー
    bool isWaitingForStart_ = true; // 開始前オフセット待機中かどうか


    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス 弱参照

};