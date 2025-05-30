#include "Note.h"

#include <Features/LineDrawer/LineDrawer.h>

void Note::Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex)
{
    model_ = std::make_unique<ObjectModel>("note");
    model_->Initialize("cube/cube.obj");

    model_->translate_ = _position;
    model_->useQuaternion_ = true;

    model_->scale_.x = 0.7f;
    model_->scale_.z = 0.5f;
    model_->scale_.y = 0.1f;

    speed_ = _speed;
    targetTime_ = _targetTime;
    laneIndex_ = _laneIndex;

    model_->Update();
}

void Note::Update(float _deltaTime)
{
    model_->translate_ += direction * speed_ * _deltaTime;

    model_->Update();
}

void Note::Draw(const Camera* _camera)
{
    model_->Draw(_camera, 0, color_);
}

void NomalNote::Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex)
{
    Note::Initilize(_position, _speed, _targetTime,_laneIndex);
}

void NomalNote::Update(float _deltaTime)
{
    Note::Update(_deltaTime);
}

void NomalNote::Draw(const Camera* _camera)
{
    Note::Draw(_camera);
}

LongNote::~LongNote()
{
    if (beforeNote_)
    {
        beforeNote_.reset();
    }
}

void LongNote::Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex)
{
    Note::Initilize(_position, _speed, _targetTime, _laneIndex);

    noteBridge_ = std::make_unique<ObjectModel>("noteBridge");
    noteBridge_->Initialize("pY1x1p01Plane");// y+向きpivot(010)
    noteBridge_->useQuaternion_ = true;
}

void LongNote::Update(float _deltaTime)
{
    Note::Update(_deltaTime);


    if (beforeNote_)
    {
        Vector3 spos = model_->translate_;
        Vector3 epos = beforeNote_->GetPosition();

        const Vector3 downVector = Vector3(0, 0, -1);// 下向きベクトル
        Vector3 direction = epos - spos;
        length_ = direction.Length();
        direction = direction.Normalize();

        noteBridge_->quaternion_ = Quaternion::FromToRotation(downVector, direction);

        noteBridge_->scale_.z = length_;

        noteBridge_->translate_ = spos;

        noteBridge_->Update();

    }
}

void LongNote::Draw(const Camera* _camera)
{
    model_->Draw(_camera,0, Vector4(0.0f, 1.0f, 0.5f, 1.0f));

    if (beforeNote_)
    {
        noteBridge_->Draw(_camera, 0, Vector4(0.5f, 1.0f, 0.5f, 1.0f));
    }
}

void LongNote::Judge()
{
    Note::Judge();

    // ブリッジを画面外へ運ぶために
    model_->translate_.z = -100.0f; // ノーツを画面外へ

}
