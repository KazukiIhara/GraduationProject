#include "Boar.h"
using namespace LGF;

void Boar::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 5.0f;
	hp_ = 700;
	color_ = Color::Purple;
}

void Boar::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//進む
	position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_ * (float)LGF::System::DeltaTime();//ベクトル正規化、スピードに合わせ移動
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
}
