#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Model/ObjectModel.h>


#include <cstdint>

class Note
{
public:

    Note() = default;
    virtual ~Note() = default;

   virtual void Initilize(const Vector3 _position,float _speed,float _targetTime, uint32_t _laneIndex);
   virtual void Update(float _deltaTime);
   virtual void Draw(const Camera* _camera);

   float GetTargetTime() const { return targetTime_; }

   Vector3 GetPosition() const { return model_->translate_; }

   uint32_t GetLaneIndex() const { return laneIndex_; }

   bool IsJudged() const { return isJudged_; }
   void Judge() { isJudged_ = true; }

protected:

    std::unique_ptr<ObjectModel> model_ = nullptr;
    Vector4 color_ = { 1,1,1,1 };

    uint32_t laneIndex_ = 0;

    float speed_ = 0.0f;

    float targetTime_ = 0.0f;

    const Vector3 direction = { 0,0,-1 };

    bool isJudged_ = false;
};


class NomalNote : public Note
{
public:
    NomalNote() = default;
    ~NomalNote() override = default;


    void Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex) override;
    void Update(float _deltaTime) override;
    void Draw(const Camera* _camera) override;


private:



};

class LongNote : public Note
{
public:
    LongNote() = default;
    ~LongNote() override;
    void Initilize(const Vector3 _position, float _speed, float _targetTime, uint32_t _laneIndex) override;
    void Update(float _deltaTime) override;
    void Draw(const Camera* _camera) override;

    void SetBeforeNote(std::shared_ptr<Note> _beforeNote) {
        beforeNote_= _beforeNote;
    }
private:

    std::shared_ptr<Note> beforeNote_ = nullptr;  // 前のノーツ
    std::unique_ptr<ObjectModel> noteBridge_ = nullptr; // ノーツブリッジ

    float length_ = 0.0f; // ノーツの長さ

};
