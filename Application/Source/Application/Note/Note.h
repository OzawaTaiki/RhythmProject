#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Model/ObjectModel.h>


#include <cstdint>

class Note
{
public:

    Note() = default;
    virtual ~Note() = default;

   virtual void Initilize(const Vector3 _position,float _speed);
   virtual void Update(float _deltaTime);
   virtual void Draw(const Camera* _camera);


protected:

    std::unique_ptr<ObjectModel> model_ = nullptr;
    Vector4 color_ = { 1,1,1,1 };

    float speed_ = 0.0f;

    const Vector3 direction = { 0,0,-1 };
};


class nomalNote : public Note
{
public:
    nomalNote() = default;
    ~nomalNote() override = default;


    void Initilize(const Vector3 _position, float _speed) override;
    void Update(float _deltaTime) override;
    void Draw(const Camera* _camera) override;

private:



};
