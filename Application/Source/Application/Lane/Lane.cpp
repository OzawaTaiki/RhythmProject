#include "Lane.h"

#include <Features/Camera/Camera/Camera.h>

float Lane::laneLength_ = 100.0f; // 初期値
float Lane::totalWidth_ = 8.0f; // 初期値
int32_t Lane::laneCount_ = 4; // 初期値
float Lane::laneWidth_ = Lane::totalWidth_ / static_cast<float>(Lane::laneCount_); // 一本あたりの幅

Lane::~Lane()
{
    notes_.clear(); // 明示的にクリア
}

void Lane::Initialize(const std::list<NoteData>& _noteDataList, int32_t _laneIndex, float _judgeLine, float  _speed, float _startOffsetTime)
{
    endPosition_ = GetLaneEndPosition(_laneIndex, _judgeLine); // レーンの開始位置を取得

    startPosition_ = endPosition_;
    startPosition_.z += laneLength_;


    CreateLaneModel();

    // ノーツを生成
    CreateNotes(_noteDataList, _laneIndex, _judgeLine, _speed, _startOffsetTime);

}

void Lane::Update(float _elapseTime, float _speed)
{
    for (auto it = notes_.begin(); it != notes_.end();)
    {
        auto& note = *it;
        if (note->IsJudged())
        {
            it = notes_.erase(it); // 判定済みのノーツは削除
            continue;
        }

        note->Update(_elapseTime,_speed);
        ++it; // 次のノーツへ
    }
}


void Lane::Draw(const Camera* _camera) const
{
    if (laneModel_)
    {
        laneModel_->Draw(_camera, { 0.5f,0.5f,0.5f,0.7f });
    }

    for (const auto& note : notes_)
    {
        note->Draw(_camera);
    }
}

Note* Lane::GetFirstNote() const
{
    if (notes_.empty())
    {
        return nullptr; // ノーツがない場合はnullptrを返す
    }

    return notes_.front().get(); // 最初のノーツを返す
}

int32_t Lane::DeleteNotesOutOfScreen(float _noteDeletePos)
{
    int32_t deleteCount = 0;
    for (auto it = notes_.begin(); it != notes_.end();)
    {
        auto& note = *it;
        if (note->GetPosition().z < _noteDeletePos) // 画面外のノーツ
        {
            note->Judge(); // ノーツを判定済みにする 念のため
            it = notes_.erase(it); // ノーツを削除
            ++deleteCount; // 削除カウントを増やす
        }
        else
        {
            // 時間順に並んでいるからここにきたら以降のノートは画面内にあるはず
            break;
        }
    }
    return deleteCount; // 削除したノーツの数を返す

}

Vector3 Lane::GetLaneEndPosition(int32_t _laneIndex, float _judgeLine)
{
    // レーンの座標を計算
    // 基準は(0,0,_judgeLine)
    static Vector3 basePosition = Vector3(0.0f, 0.0f, _judgeLine);
    static float laneLeftEdge = basePosition.x - (totalWidth_ / 2.0f);

    Vector3 endPosition;

    endPosition.x = laneLeftEdge + (static_cast<float>(_laneIndex) * laneWidth_) + laneWidth_ / 2.0f;
    endPosition.y = 0.0f; // 高さは0
    endPosition.z = basePosition.z; // 奥

    return endPosition; // レーンの開始位置を返す
}

void Lane::CreateNotes(const std::list<NoteData>& _noteDataList, int32_t _laneIndex, float _judgeLine, float  _speed, float _startOffsetTime)
{
    notes_.clear();

    Vector3 noteTargetPosition = endPosition_;
    startPosition_.z = _judgeLine; // レーンの開始位置のZ座標を判定ラインに合わせる
    startPosition_.y = 0.0f; // 高さは0
    Vector3 noteStartPosition = startPosition_;


    // _noteDataListは時間でソート済み
    for (const auto& noteData : _noteDataList)
    {
        if (noteData.laneIndex == _laneIndex) // 念のためチェック
        {
            // ノーツを生成して追加
            if(noteData.noteType =="normal")
            {
                auto note = std::make_shared<NomalNote>();

                note->Initilize(noteData.targetTime ,noteTargetPosition);
                notes_.push_back((note));
            }
            else if (noteData.noteType == "hold")
            {
                // 前のノートを生成
                auto note = std::make_shared<LongNote>();
                note->Initilize(noteData.targetTime, noteTargetPosition);
                note->SetHoldEnd(false); // ブリッジはまだ生成しない
                notes_.push_back((note));

                // ホールド終端を生成
                auto holdEndNote = std::make_shared<LongNote>();
                holdEndNote->Initilize(noteData.targetTime + noteData.holdDuration, noteTargetPosition);
                holdEndNote->SetHoldEnd(true); // ブリッジを生成するフラグを立てる
                holdEndNote->SetHoldDuration(noteData.holdDuration); // ホールド時間を設定
                notes_.push_back((holdEndNote));
            }
        }
    }
}

void Lane::CreateLaneModel()
{
    laneModel_ = std::make_unique<ObjectModel>("Lane");
    laneModel_->Initialize("pY1x1p01Plane");
    laneModel_->scale_ = Vector3(laneWidth_ - 0.1f, 1.0f, laneLength_); // レーンの幅と長さを設定
    laneModel_->translate_ = startPosition_;
    laneModel_->translate_.y -= 0.001f; // zファイティング対策で少し下げる
    laneModel_->Update();
}
