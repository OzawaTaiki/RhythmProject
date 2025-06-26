#include "Note.h"

#include <Features/LineDrawer/LineDrawer.h>

#include <Debug/Debug.h>

void Note::Initilize(float _targetTime, const Vector3& _targetPosition)
{
    model_ = std::make_unique<ObjectModel>("note");
    model_->Initialize("cube/cube.obj");

    model_->translate_ = Vector3(0, 0, 0);
    model_->useQuaternion_ = true;

    model_->scale_.x = 0.7f;
    model_->scale_.z = 0.5f;
    model_->scale_.y = 0.1f;

    targetTime_ = _targetTime;
    targetPosition_ = _targetPosition;

    model_->Update();
}

void Note::Update(float _elapseTime, float _speed)
{
    model_->translate_ = targetPosition_;
    model_->translate_.z = targetPosition_.z + _speed * (targetTime_ - _elapseTime);

    model_->Update();
}

void Note::Draw(const Camera* _camera)
{
    model_->Draw(_camera, 0, color_);
}

void Note::Judge()
{
    isJudged_ = true; // 判定済みにする
    //TODO: 音鳴らす
    // event発行？soundInstance持たせる？
}

NomalNote::NomalNote() :    Note()
{
    noteType_ = NoteType::Normal;
    color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

NomalNote::~NomalNote()
{
}

void NomalNote::Initilize(float _targetTime, const Vector3& _targetPosition)
{
    Note::Initilize(_targetTime, _targetPosition);
}

void NomalNote::Update(float _elapseTime, float _speed)
{
    Note::Update(_elapseTime, _speed);
}

void NomalNote::Draw(const Camera* _camera)
{
    Note::Draw(_camera);
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

void LongNote::Initilize(float _targetTime, const Vector3& _targetPosition)
{
    Note::Initilize(_targetTime, _targetPosition);
}

void LongNote::Update(float _elapseTime, float _speed)
{
    Note::Update(_elapseTime, _speed);

    if (noteBridge_)
    {
        Vector3 spos = model_->translate_;
        Vector3 epos = spos;
        epos.z -= _speed * (holdDuration_);
        Vector3 direction = epos - spos;

        // レーンマタギを実装したら必要
        /*const Vector3 downVector = Vector3(0, 0, -1);// 下向きベクトル
        direction = direction.Normalize();

        noteBridge_->quaternion_ = Quaternion::FromToRotation(downVector, direction);*/

        noteBridge_->scale_.z = direction.Length();

        noteBridge_->translate_ = spos;

        noteBridge_->Update();

    }
}

void LongNote::Draw(const Camera* _camera)
{
     model_->Draw(_camera,0, Vector4(0.0f, 1.0f, 0.5f, 1.0f));

    // 次のノートが有効な場合はブリッジを描画する
    if (noteBridge_)
    {
        noteBridge_->Draw(_camera, 0, Vector4(0.5f, 1.0f, 0.5f, 1.0f));
    }
}

void LongNote::Judge()
{
    // 次のノートが無効になったらフラグをたてる
    Note::Judge();

    // ブリッジを画面外へ運ぶために
    model_->translate_.z = -2; // ノーツを画面外へ
}

void LongNote::SetHoldEnd(bool _isHoldEnd)
{
    isHoldEnd_ = _isHoldEnd;

    if (isHoldEnd_)
    {
        noteBridge_ = std::make_unique<ObjectModel>("noteBridge");
        noteBridge_->Initialize("pY1x1p01Plane");// y+向きpivot(010)
        noteBridge_->useQuaternion_ = true;

        noteType_ = NoteType::HoldEnd; // ノーツタイプを設定
    }
}
