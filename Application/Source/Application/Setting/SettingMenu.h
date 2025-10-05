#pragma once

#include <Features/UI/UIGroup.h>

class SettingMenu
{
public:
    SettingMenu() = default;
    ~SettingMenu() = default;
    // 初期化
    void Initialize(std::function<void(float)> _speedSetFunc, std::function<void(float)> _audioLatencySetFunc = nullptr);
    // 更新
    void Update();
    // 描画
    void Draw();
    // メニューがアクティブかどうか
    bool IsActive() const { return isActive_; }

private:

    bool isActive_ = false; // メニューがアクティブかどうか

    std::unique_ptr<UIGroup> uiGroup_ = nullptr; // UIグループ

    std::function<void(float)> speedSetFunc_ = nullptr; // ノーツ速度を設定するコールバック関数
    std::function<void(float)> audioLatencySetFunc_ = nullptr; // 音声遅延を設定するコールバック関数


    std::vector<std::shared_ptr<UISlider>> sliders_; // スライダー   のリスト

};