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
    model_->scale_.y = 0.3f;

    speed_ = _speed;
    targetTime_ = _targetTime;
    laneIndex_ = _laneIndex;
}

void Note::Update(float _deltaTime)
{
    model_->translate_ += direction * speed_ * _deltaTime;

    model_->Update();
}

void Note::Draw(const Camera* _camera)
{
    model_->Draw(_camera, color_);
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
    if (nextNote_)
    {
        nextNote_.reset();
    }
}

void LongNote::Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex)
{
    Note::Initilize(_position, _speed, _targetTime, _laneIndex);
}

void LongNote::Update(float _deltaTime)
{
    Note::Update(_deltaTime);
}

void LongNote::Draw(const Camera* _camera)
{
    Note::Draw(_camera);

    if(nextNote_)
    {
        Vector3 spos = model_->translate_;
        Vector3 epos = nextNote_->GetPosition();

        LineDrawer::GetInstance()->RegisterPoint(spos, epos, Vector4(1, 1, 0, 1));
    }
}
