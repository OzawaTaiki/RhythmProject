#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Model/ObjectModel.h>


#include <cstdint>

class Note
{
public:

    Note() = default;
    virtual ~Note() = default;

    virtual void Initilize(const Vector3 _position, float _targetTime, float _generatedTime, const Vector3& _targetPosition, uint32_t _laneIndex);
   virtual void Update(float _elapseTime);
   virtual void Draw(const Camera* _camera);

   float GetTargetTime() const { return targetTime_; }

   Vector3 GetPosition() const { return model_->translate_; }

   uint32_t GetLaneIndex() const { return laneIndex_; }

   bool IsJudged() const { return isJudged_; }
   virtual void Judge();

protected:

    std::unique_ptr<ObjectModel> model_ = nullptr;
    Vector4 color_ = { 1,1,1,1 };

    float targetTime_ = 0.0f;

    uint32_t laneIndex_ = 0; // レーンインデックス

    float generateTime_ = 0.0f; // ノーツが生成された時間
    Vector3 generatePosition_ = { 0,0,0 }; // ノーツが生成された位置
    Vector3 targetPosition_ = { 0,0,0 }; // ノーツの目標位置

    bool isJudged_ = false;
};


class NomalNote : public Note
{
public:
    NomalNote() = default;
    ~NomalNote() override = default;


    void Initilize(const Vector3 _position, float _targetTime, float _generatedTime, const Vector3& _targetPosition, uint32_t _laneIndex) override;
    void Update(float _elapseTime) override;
    void Draw(const Camera* _camera) override;


private:



};

class LongNote : public Note
{
public:
    LongNote() = default;
    ~LongNote() override;
    void Initilize(const Vector3 _position, float _targetTime, float _generatedTime, const Vector3& _targetPosition, uint32_t _laneIndex) override;
    void Update(float _elapseTime) override;
    void Draw(const Camera* _camera) override;

    virtual void Judge() override;

    void SetBeforeNote(std::shared_ptr<Note> _beforeNote) {
        beforeNote_= _beforeNote;
    }
private:

    std::shared_ptr<Note> beforeNote_ = nullptr;  // 前のノーツ
    std::unique_ptr<ObjectModel> noteBridge_ = nullptr; // ノーツブリッジ

    float length_ = 0.0f; // ノーツの長さ

};
