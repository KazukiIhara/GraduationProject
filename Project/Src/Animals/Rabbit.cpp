#include "Rabbit.h"
#include "AppMath/AppMath.h"

using namespace LGF;

void Rabbit::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 0.05f;
	hp_ = 600;
}

void Rabbit::Update() {
	BaseAnimal::Update();
	hp_--;
}

void Rabbit::Draw() {

	Matrix4x4 translate = Math::MakeTranslateMatrix(position_);
	Matrix4x4 rotate = Math::MakeRotateXMatrix(rotate_.x) * Math::MakeRotateYMatrix(rotate_.y) * Math::MakeRotateZMatrix(rotate_.z);
	Matrix4x4 scale = Math::MakeScaleMatrix({1,1,1});

	object_.Draw(scale * rotate * translate, Vector4{ 0.0f,1.0f,0.0f,1.0f });
}

void Rabbit::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	
	if (jumpTimer_ <= 0.0f) {
		startPoint = position_;
		halfPoint = position_ + Math::Normalize(differenceCharHan);
		halfPoint.y = kJumpCurvePointY_;
		endPoint = (position_ + Math::Normalize(differenceCharHan)) / 1.2f;
	}
	jumpTimer_ += 1.0f / 60.0f;

	Vector3 p0p1 = AppMath::Lerp(startPoint, halfPoint, jumpTimer_);
		
	Vector3 p1p2 = AppMath::Lerp(halfPoint, endPoint, jumpTimer_);
		
	//進む
	position_ = AppMath::Lerp(p0p1, p1p2, jumpTimer_);//ベクトル正規化、スピードに合わせ移動
	
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);

	if (jumpTimer_ >= 1.0f) {
		jumpTimer_ = 0.0f;
	}
}

