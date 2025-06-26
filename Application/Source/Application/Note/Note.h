#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Model/ObjectModel.h>


#include <cstdint>

class Note
{
public:

    Note() = default;
    virtual ~Note() = default;

    virtual void Initilize(float _targetTime, const Vector3& _targetPosition);
    virtual void Update(float _elapseTime, float _speed);
    virtual void Draw(const Camera* _camera);

    float GetTargetTime() const { return targetTime_; }

    Vector3 GetPosition() const { return model_->translate_; }

    bool IsJudged() const { return isJudged_; }
    virtual void Judge();

protected:

    std::unique_ptr<ObjectModel> model_ = nullptr;
    Vector4 color_ = { 1,1,1,1 };

    float targetTime_ = 0.0f;

    Vector3 targetPosition_ = { 0,0,0 }; // ノーツの目標位置

    bool isJudged_ = false;
};


class NomalNote : public Note
{
public:
    NomalNote() = default;
    ~NomalNote() override;


    void Initilize(float _targetTime, const Vector3& _targetPosition) override;
    void Update(float _elapseTime, float _speed) override;
    void Draw(const Camera* _camera) override;


private:



};

class LongNote : public Note
{
public:
    LongNote() = default;
    ~LongNote() override;

    void Initilize(float _targetTime, const Vector3& _targetPosition) override;
    void Update(float _elapseTime, float _speed) override;
    void Draw(const Camera* _camera) override;

    virtual void Judge() override;

    void SetHoldEnd(bool _isHoldEnd);
    bool IsHoldEnd() const { return isHoldEnd_; }

    void SetHoldDuration(float _holdDuration) { holdDuration_ = _holdDuration; }
    float GetHoldDuration() const { return holdDuration_; }

private:

    std::unique_ptr<ObjectModel> noteBridge_ = nullptr; // ノーツブリッジ

    bool isHoldEnd_ = false; // ロングノート終端かどうか
    float holdDuration_ = 0.0f; // ホールド時間

};
