#include "Note.h"


void Note::Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex)
{
    model_ = std::make_unique<ObjectModel>("note");
    model_->Initialize("cube/cube.obj");

    model_->translate_ = _position;
    model_->useQuaternion_ = true;

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

void nomalNote::Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex)
{
    Note::Initilize(_position, _speed, _targetTime,_laneIndex);
}

void nomalNote::Update(float _deltaTime)
{
    Note::Update(_deltaTime);
}

void nomalNote::Draw(const Camera* _camera)
{
    Note::Draw(_camera);
}
