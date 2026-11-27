#include "Deer.h"
#include "Collision/CollisionManager.h"

using namespace LGF;

void Deer::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//進む
	position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_ * (float)LGF::System::DeltaTime();//ベクトル正規化、スピードに合わせ移動
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
}

void Deer::Attack(Target target) {
	if (IsAnticipationTime(target, 2.0f) || isAttack_) {
		attackTime_ += (float)System::DeltaTime();

		//攻撃 噛みつく
		attackPoint_ = position_ + LGF::Math::Normalize(bitePoint_) * moveSpeed_;//一歩前を攻撃

		if (attackTime_ >= KAttackTimeMax_) {
			attackTime_ = 0.0f;
			AttackFinishCommon();
		}
	}
	else {
		//予備動作

		//噛みつく(bite)場所を受け取る
		switch (target)
		{
		case charhan:
			bitePoint_ = chaHanPosition_ - position_;
			break;
		case player:
			bitePoint_ = playerPosition_ - position_;
			break;
		default:
			break;
		}
	}
}