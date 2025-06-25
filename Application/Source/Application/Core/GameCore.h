#pragma once


class Camera;

// ゲームの核となる部分

class GameCore
{
public:

    // コンストラクタ
    GameCore();
    // デストラクタ
    ~GameCore();


    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理
    /// </summary>
    // 後々/ <param name="_inputData">入力データ</param>
    void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="_camera">カメラ</param>
    void Draw(const Camera* _camera);



private:

    // note
    // lane
    // judge

    // score あとまわし

    // note spawner あったらいいな


};