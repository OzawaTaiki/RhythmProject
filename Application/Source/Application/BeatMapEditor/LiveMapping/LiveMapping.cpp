#include "LiveMapping.h"

void LiveMapping::Initialize(int32_t _laneCount)
{
    input_ = Input::GetInstance(); // 入力管理クラスのインスタンスを取得

    // レーン数に応じてキーコードのリストを初期化
    laneKeyBindings_.resize(_laneCount, 0); // 初期値は0（未設定）
    //デフォルトを設定
    SetDefaultKeyBindings(_laneCount);

    // マッピングデータの初期化
    mappingData_.clear();
    mappingData_.reserve(100); // 初期容量を設定（必要に応じて調整可能）

}

void LiveMapping::Update(float _elapsedTime)
{
    for (size_t i = 0; i < laneKeyBindings_.size(); ++i)
    {
        // キーが押されたかチェック
        if (input_->IsKeyTriggered(laneKeyBindings_[i]))
        {
            // マッピングデータに追加
            mappingData_.emplace_back(i, _elapsedTime);
        }
    }
}

void LiveMapping::ResetMappingData()
{
    mappingData_.clear(); // マッピングデータをクリア
}

void LiveMapping::SetLaneKeyBinding(int32_t _laneIndex, uint8_t _keyCode)
{
    // レーンインデックスが範囲内かチェック
    if (_laneIndex < 0 || _laneIndex >= static_cast<int32_t>(laneKeyBindings_.size()))
    {
        return;
    }

    laneKeyBindings_[_laneIndex] = _keyCode; // キーコードを設定
}

void LiveMapping::SetDefaultKeyBindings(int32_t _laneIndex)
{
     // 使わないかもだけど念のため複数パターン用意しておく
    if (_laneIndex == 4)
    {
        laneKeyBindings_[0] = DIK_D;
        laneKeyBindings_[1] = DIK_F;
        laneKeyBindings_[2] = DIK_J;
        laneKeyBindings_[3] = DIK_K;
    }
    if (_laneIndex == 5)
    {
        laneKeyBindings_[0] = DIK_D;
        laneKeyBindings_[1] = DIK_F;
        laneKeyBindings_[2] = DIK_SPACE;
        laneKeyBindings_[3] = DIK_J;
        laneKeyBindings_[4] = DIK_K;
    }
    if (_laneIndex == 6)
    {
        laneKeyBindings_[0] = DIK_S;
        laneKeyBindings_[1] = DIK_D;
        laneKeyBindings_[2] = DIK_F;
        laneKeyBindings_[3] = DIK_J;
        laneKeyBindings_[4] = DIK_K;
        laneKeyBindings_[5] = DIK_L;
    }
}
