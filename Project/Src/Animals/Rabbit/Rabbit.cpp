#include "Rabbit.h"
#include "AppMath/AppMath.h"

using namespace LGF;

void Rabbit::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 2.0f;
	hp_ = 600;
	color_ = Color::Green;
}

void Rabbit::Move() {
	jumpDelayTimer_ += (float)LGF::System::DeltaTime();
	if (jumpDelayTimer_ <= kJumpDelayTimeFinish_) {
		return;
	}

	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	
	if (jumpTimer_ <= 0.0f) {
		startPoint = position_;
		halfPoint = position_ + Math::Normalize(differenceCharHan);
		halfPoint.y = kJumpCurvePointY_;
		endPoint = position_ + (Math::Normalize(differenceCharHan) * moveSpeed_);
	}
	jumpTimer_ += (float)LGF::System::DeltaTime();

	Vector3 p0p1 = AppMath::Lerp(startPoint, halfPoint, jumpTimer_);
		
	Vector3 p1p2 = AppMath::Lerp(halfPoint, endPoint, jumpTimer_);
		
	//進む
	position_ = AppMath::Lerp(p0p1, p1p2, jumpTimer_);//ベクトル正規化、スピードに合わせ移動
	
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);

	if (jumpTimer_ >= kJumpTimeFinish_) {
		jumpTimer_ = 0.0f;
		jumpDelayTimer_ = 0.0f;
	}
}

