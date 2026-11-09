#include "Deer.h"

using namespace LGF;

void Deer::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 3.0f;
	hp_ = 400;
}

void Deer::Draw() {
	Matrix4x4 translate = Math::MakeTranslateMatrix(position_);
	Matrix4x4 rotate = Math::MakeRotateXMatrix(rotate_.x) * Math::MakeRotateYMatrix(rotate_.y) * Math::MakeRotateZMatrix(rotate_.z);
	Matrix4x4 scale = Math::MakeScaleMatrix({ 1,1,1 });

	object_.Draw(scale * rotate * translate, Vector4{ 1.0f,0.0f,0.0f,1.0f });
}

void Deer::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//進む
	position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_ * (float)LGF::System::DeltaTime();//ベクトル正規化、スピードに合わせ移動
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
}