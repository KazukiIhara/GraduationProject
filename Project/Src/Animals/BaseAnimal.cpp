#include "BaseAnimal.h"

void BaseAnimal::Update() {
	GoToTarget();
	Dead();
}

void BaseAnimal::SetPlayerPosition(const LGF::Vector3& playerPos) {
	playerPosition_ = playerPos;//仮
}

void BaseAnimal::SetCharHanPosition(const LGF::Vector3& playerPos) {
	targetPosition_ = playerPos;//仮
}

void BaseAnimal::GoToTarget() {
	if (hp_ <= 0) { return; }
	
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = targetPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//炒飯とのベクトルの大きさ
	float dot = LGF::Math::Length(differenceCharHan);
	
	//プレイヤーとの位置関係
	LGF::Vector3 differencePlayer = playerPosition_ - position_;//ターゲットと現在位置の差分ベクトル	
	//炒飯とのベクトルの大きさ
	float dot2 = LGF::Math::Length(differencePlayer);
	
	
	//対象物との距離が一定以上でないなら進む
	if (dot2 <= attackRadius_) {
		differencePlayer = LGF::Math::Normalize(differencePlayer);//ベクトル正規化
		//向き
		rotate_.y = std::atan2(differencePlayer.x, differencePlayer.z);
	}
	else if (dot >= attackRadius_) {
		position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_;//ベクトル正規化、スピードに合わせ移動
		//向き
		rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
	}
	

}

void BaseAnimal::Dead() {
	if (hp_ > 0) { return; }

	if (rotate_.x >= -1.25f) {
		rotate_.x -= 0.05f;
	}
	deadAnimationTime_ += 1.0f / 60.0f;
}
