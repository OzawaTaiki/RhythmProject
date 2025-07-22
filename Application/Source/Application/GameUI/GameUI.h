#pragma once

//#include <Features/UI/UISprite.h>
#include <Features/TextRenderer/TextGenerator.h>
#include <Features/Json/JsonBinder.h>

class GameUI
{
public:

    GameUI() = default;
    ~GameUI() = default;

    // 初期化
    void Initialize();

    // 更新処理
    void Update(int32_t _combo);

    // 描画処理
    void Draw();


private:

    void ImGui();

private:
    TextGenerator textGenerator_; // テキスト生成用のインスタンス

    TextParam comboValueParam_; // コンボ値のテキストパラメータ
    TextParam comboTextParam_; // コンボのテキストパラメータ

    int32_t comboValue_ = 0; // コンボ値

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr; // JSONバインダー
};