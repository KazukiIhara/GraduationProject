#include "BaseAnimal.h"

void BaseAnimal::SetPlayerPosition(const LGF::Vector3& playerPos) {
	targetPosition_ = playerPos;//仮
}

void BaseAnimal::GoToTarget() {
	if (hp_ <= 0) { return; }

	LGF::Vector3 difference = targetPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	difference = LGF::Math::Normalize(difference);//ベクトル正規化
	
	position_ += difference * moveSpeed_;//スピードに合わせ移動
}

void BaseAnimal::Dead() {
	if (hp_ <= 0) {
		deadAnimationTime_ += 1.0f / 60.0f;
		return;
	}
	hp_--;
}
