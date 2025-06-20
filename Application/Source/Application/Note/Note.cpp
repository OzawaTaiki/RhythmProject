#include "Note.h"

#include <Features/LineDrawer/LineDrawer.h>

void Note::Initilize(const Vector3 _position, float _targetTime, float _generatedTime, const Vector3& _targetPosition, uint32_t _laneIndex)
{
    model_ = std::make_unique<ObjectModel>("note");
    model_->Initialize("cube/cube.obj");

    model_->translate_ = _position;
    model_->useQuaternion_ = true;

    model_->scale_.x = 0.7f;
    model_->scale_.z = 0.5f;
    model_->scale_.y = 0.1f;

    laneIndex_ = _laneIndex;

    targetTime_ = _targetTime;
    generateTime_ = _generatedTime;
    generatePosition_ = _position;
    targetPosition_ = _targetPosition;

    model_->Update();
}

void Note::Update(float _elapseTime)
{
    float t = _elapseTime / targetTime_;
    model_->translate_ = Vector3::Lerp(generatePosition_, targetPosition_, t);

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

void NomalNote::Initilize(const Vector3 _position, float _targetTime, float _generatedTime, const Vector3& _targetPosition, uint32_t _laneIndex)
{
    Note::Initilize(_position, _targetTime, _generatedTime, _targetPosition, _laneIndex);
}

void NomalNote::Update(float _elapseTime)
{
    Note::Update(_elapseTime);
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

void LongNote::Initilize(const Vector3 _position, float _targetTime, float _generatedTime, const Vector3& _targetPosition, uint32_t _laneIndex)
{
    Note::Initilize(_position, _targetTime, _generatedTime, _targetPosition,_laneIndex);

    noteBridge_ = std::make_unique<ObjectModel>("noteBridge");
    noteBridge_->Initialize("pY1x1p01Plane");// y+向きpivot(010)
    noteBridge_->useQuaternion_ = true;
}

void LongNote::Update(float _elapseTime)
{
    Note::Update(_elapseTime);


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
        // ノーツ間の差分ベクトルを計算
        Vector3  sub = beforeNote_->GetPosition() - model_->translate_;
        // 前のノーツが自身より奥にある場合描画しない
        if (sub.z >= 0)
        {
            return;
        }

        noteBridge_->Draw(_camera, 0, Vector4(0.5f, 1.0f, 0.5f, 1.0f));
    }
}

void LongNote::Judge()
{
    Note::Judge();

    // ブリッジを画面外へ運ぶために
    model_->translate_.z = -2; // ノーツを画面外へ

}
