#include "Note.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Math/Matrix/MatrixFunction.h>
#include <Debug/Debug.h>

using namespace Engine;

namespace
{
constexpr float kNoteScaleX = 0.7f;
constexpr float kNoteScaleZ = 0.5f;
constexpr float kNoteScaleY = 0.001f;
}

void Note::Initialize(float targetTime, const Engine::Vector3& targetPosition)
{
    model_ = std::make_unique<ObjectModel>("note");
    model_->Initialize("cube/cube.obj");

    model_->translate_ = Vector3(0, 0, 0);
    model_->useQuaternion_ = true;

    // TODO : 専用のモデルを用意する
    model_->scale_.x = kNoteScaleX;
    model_->scale_.y = kNoteScaleY;
    model_->scale_.z = kNoteScaleZ;

    targetTime_ = targetTime;
    targetPosition_ = targetPosition;

    model_->Update();
}

void Note::Update(float elapseTime, float speed)
{
    model_->translate_ = targetPosition_;
    model_->translate_.z = targetPosition_.z + speed * (targetTime_ - elapseTime);

    model_->Update();
}

Matrix4x4 Note::GetWorldMatrix() const
{
    return MakeAffineMatrix(model_->scale_, model_->quaternion_, model_->translate_);
}

void Note::Judge()
{
    isJudged_ = true; // 判定済みにする
}

NormalNote::NormalNote() : Note()
{
    noteType_ = NoteType::Normal;
    color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

NormalNote::~NormalNote()
{
}

void NormalNote::Initialize(float targetTime, const Vector3& targetPosition)
{
    Note::Initialize(targetTime, targetPosition);
}

void NormalNote::Update(float elapseTime, float speed)
{
    Note::Update(elapseTime, speed);
}

void NormalNote::AddToDrawer(NoteDrawer* drawer) const
{
    drawer->AddNote(GetWorldMatrix(), color_);
}

LongNote::LongNote() : Note()
{
    noteType_ = NoteType::Hold;
    color_ = Vector4(0.0f, 1.0f, 0.5f, 1.0f); // 緑色
    holdDuration_ = 0.0f; // デフォルトのホールド時間
    isHoldEnd_ = false;
}

LongNote::~LongNote()
{
}

void LongNote::Initialize(float targetTime, const Vector3& targetPosition)
{
    Note::Initialize(targetTime, targetPosition);
}

void LongNote::Update(float elapseTime, float speed)
{
    Note::Update(elapseTime, speed);
    if (noteBridge_)
    {
        // ブリッジの長さを更新する
        Vector3 spos = model_->translate_;
        Vector3 epos = spos;
        // TODO : ホールド中のみ判定ラインで切れるようにする
        /*実装こんな感じ？わからん
        bool isHolding = false; // ホールド中かどうかのフラグを取得する処理を実装する
        if(isHolding)
            epos = judgePosition_; // ホールド中は判定位置まで伸ばす
        else
            epos.z = targetPosition_.z + speed * (targetTime_ - elapseTime); // ホールド終了後は自分の位置まで
        */
        float bridgeLength = holdDuration_;
        if (isHeadPressed_ && holdDuration_ > targetTime_ - elapseTime)
            bridgeLength = targetTime_ - elapseTime;
        epos.z -= speed * (bridgeLength);
        Vector3 direction = epos - spos;
        // レーンマタギを実装したら必要
        /*const Vector3 downVector = Vector3(0, 0, -1);// 下向きベクトル
        direction = direction.Normalize();

        noteBridge_->quaternion_ = Quaternion::FromToRotation(downVector, direction);*/

        noteBridge_->scale_.z = direction.Length(); // ブリッジの長さを設定

        noteBridge_->translate_ = spos; // ブリッジの位置をノーツの位置に設定

        noteBridge_->Update();

    }
}

void LongNote::AddToDrawer(NoteDrawer* drawer) const
{
    drawer->AddNote(GetWorldMatrix(), Vector4(0.0f, 1.0f, 0.5f, 1.0f)); // ノーツを描画
    if (noteBridge_)
    {
        Matrix4x4 bridgeWorld = MakeAffineMatrix(noteBridge_->scale_, noteBridge_->quaternion_, noteBridge_->translate_);
        drawer->AddBridge(bridgeWorld, Vector4(0.5f, 1.0f, 0.5f, 1.0f)); // ブリッジを描画
    }
}

void LongNote::Judge()
{
    // 次のノートが無効になったらフラグをたてる
    Note::Judge();

    // ブリッジを画面外へ運ぶために
    model_->translate_.z = -2; // ノーツを画面外へ
}

void LongNote::SetHoldEnd(bool isHoldEnd)
{
    isHoldEnd_ = isHoldEnd;

    if (isHoldEnd_)
    {
        noteBridge_ = std::make_unique<ObjectModel>("noteBridge");
        noteBridge_->Initialize("pY1x1p01Plane");// y+向きpivot(010)
        noteBridge_->useQuaternion_ = true;

        isHeadPressed_ = false;

        noteType_ = NoteType::HoldEnd; // ノーツタイプを設定
    }
}
