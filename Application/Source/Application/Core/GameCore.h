#pragma once

// Engine
#include <System/Audio/VoiceInstance.h>

// Application
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/Lane/Lane.h>
#include <Application/Input/InputData.h>
#include <Application/Note/Judge/NoteJudge.h>
#include <Application/Note/Judge/JudgeResult.h>
#include <Application/GameMusic/GameMusic.h>

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
    void Initialize(float _noteSpeed, float _musicLaytencyMs = 0.0f, float _beginOffset = 2.0f);

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
    /// GameMusicを設定する
    /// </summary>
    /// <param name="_gameMusic">GameMusicのインスタンス</param>
    void SetGameMusic(const GameMusic* _gameMusic) { gamemusic_ = _gameMusic; }

    /// <summary>
    /// 開始
    /// </summary>
    void Start() { isWaitingForStart_ = false; }

    /// <summary>
    /// リスタート
    /// </summary>
    void Restart();


    /// <summary>
    /// 判定時のコールバック関数を設定する
    /// </summary>
    /// <param name="_callback">コールバック関数</param>
    void SetJudgeCallback(const std::function<void(int32_t,JudgeType)>& _callback) { onJudgeCallback_ = _callback; }

    /// <summary>
    /// ミス時のコールバック関数を設定する
    /// </summary>
    /// <param name="_callback">コールバック関数</param>
    void SetMissCallback(const std::function<void(void)>& _callback) { onMissCallback_ = _callback; }

    /// <summary>
    /// ホールド時のコールバック関数を設定する
    /// </summary>
    /// <param name="_callback">コールバック関数</param>
    void SetHoldCallback(const std::function<void(int32_t)>& _callback) { onHoldCallback_ = _callback; }


    /// <summary>
    /// ノーツの移動速度を設定する
    /// </summary>
    /// <param name="_speed">ノーツの移動速度</param>
    void SetNoteSpeed(float _speed) { noteSpeed_ = _speed; noteJudge_->SetSpeed(noteSpeed_); }

    /// <summary>
    /// 音楽の遅延を設定する
    /// </summary>
    /// <param name="_latencyMs">遅延時間（ミリ秒）</param>
    void SetMusicLatency(float _latencyMs) { musicLatencyMs_ = _latencyMs; }

    /// <summary>
    /// 最大コンボ数を取得する
    /// </summary>
    /// <returns>最大コンボ数</returns>
    int32_t GetMaxCombo() const { return maxCombo_; }

    /// <summary>
    /// 現在のコンボ数を取得する
    /// </summary>
    /// <returns>現在のコンボ数</returns>
    int32_t GetCombo() const { return combo_; }

    /// <summary>
    /// レーンの数を取得する
    /// </summary>
    /// <returns></returns>
    int32_t GetLaneCount() const { return static_cast<int32_t>(lanes_.size()); }

    /// <summary>
    /// 判定結果を取得する
    /// </summary>
    /// <returns>判定結果のマップ</returns>
    std::map<JudgeType, uint32_t> GetJudgeResult() const { return judgeResult_->GetJudgeResult(); }

private:

    void JudgeNotes(const std::vector<InputDate>& _inputData);

    JudgeType ProcessNormalNote(Note* _note, const InputDate& _inputData);

    JudgeType ProcessHoldNote(Note* _note, const InputDate& _inputData);

    JudgeType ProcessHoldEndNote(Note* _note, const InputDate& _inputData);

    void UpdateCombo(JudgeType _result);

    void RecordJudgeResult(JudgeType _result, Note* _note);

    /// <summary>
    /// ノーツを生成する
    /// </summary>
    /// <param name="_beatMapData">譜面データ</param>
    void ParseBeatMapData(const BeatMapData& _beatMapData);

    void CreateBeatMapNotes();

private:
    struct HoldState
    {
        bool isHolding = false;
        int32_t laneIndex = -1;

        void StartHold(int32_t lane) {
            isHolding = true;
            laneIndex = lane;
        }

        void EndHold() {
            isHolding = false;
            laneIndex = -1;
        }

        bool IsHoldingLane(int32_t lane) const {
            return isHolding && laneIndex == lane;
        }
    };

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


    // コンボ
    int32_t combo_ = 0; // 現在のコンボ数
    int32_t maxCombo_ = 0; // 最大コンボ数



    //-------------------------
    // コールバック関連

    // 判定時のコールバック関数
    std::function<void(int32_t, JudgeType)> onJudgeCallback_;
    // ミス時のコールバック関数
    std::function<void(void)> onMissCallback_;
    // ホールド時のコールバック関数
    std::function<void(int32_t)> onHoldCallback_;

    float noteDeletePosition_ = -10.0f; // ノーツを削除する位置

    /// 開始前オフセット関連
    float beginOffset_ = 2.0f; // ゲーム開始オフセット時間
    float waitTimer_ = 0.0f; // 開始前オフセット待機タイマー
    bool isWaitingForStart_ = true; // 開始前オフセット待機中かどうか

    float musicLatencyMs_ = 0.0f; // 音楽の遅延


    // ホールド中
    HoldState holdState_ = {};


    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス 弱参照

    const GameMusic* gamemusic_ = nullptr; // 音楽の管理

};