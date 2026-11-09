#include "Deer.h"

using namespace LGF;

void Deer::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 3.0f;
	hp_ = 400;
	color_ = Color::Red;
}

void Deer::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//進む
	position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_ * (float)LGF::System::DeltaTime();//ベクトル正規化、スピードに合わせ移動
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
}