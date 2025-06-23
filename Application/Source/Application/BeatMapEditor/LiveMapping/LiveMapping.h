#pragma once

#include <System/Input/Input.h>


#include <cstdint>
#include <vector>

struct MappingData
{
    int32_t laneIndex; // レーンインデックス
    float targetTime; // ターゲット時間

    MappingData(int32_t _laneIndex = -1, float _targetTime = -1.0f)
        : laneIndex(_laneIndex), targetTime(_targetTime) {
    }

};

class LiveMapping
{
public:

    LiveMapping() = default;
    ~LiveMapping() = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(int32_t _laneCount);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="_elapsedTime">曲の経過時間</param>
    void Update(float _elapsedTime);

    /// <summary>
    /// マッピングデータを取得
    /// </summary>
    /// <returns>マッピングデータのリスト</returns>
    std::vector<MappingData>& GetMappingData() { return mappingData_; }

    /// <summary>
    /// マッピングデータをリセット
    /// </summary>
    void ResetMappingData();

    /// <summary>
    /// レーンのキー入力バインディングを設定
    /// </summary>
    /// <param name="_laneIndex">レーン番号</param>
    /// <param name="_keyCode">キーコード (DIK_Aなど)</param>
    void SetLaneKeyBinding(int32_t _laneIndex, uint8_t _keyCode);

private:

    /// <summary>
    /// デフォルトのキー入力バインディングを設定
    /// </summary>
    void SetDefaultKeyBindings(int32_t _laneIndex);

private:
    Input* input_ = nullptr; // 入力管理クラスへのポインタ

    std::vector<uint8_t> laneKeyBindings_; // レーンごとのキー入力バインディング

    std::vector<MappingData> mappingData_; // マッピングデータのリスト

};