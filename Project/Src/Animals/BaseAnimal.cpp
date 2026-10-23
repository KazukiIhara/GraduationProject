#include "BaseAnimal.h"

void BaseAnimal::Update() {
	if (!state_) {
		state_ = std::make_unique<MoveAnimalState>();
	}

	state_->SetBaseAnimal(this);
	state_->Update();

	if (state_->ChangeState()) {
		state_ = state_->ChangeState();
	}
}

void BaseAnimal::SetPlayerPosition(const LGF::Vector3& playerPos) {
	if (isAttack_) { return; }
	playerPosition_ = playerPos;
}

void BaseAnimal::SetCharHanPosition(const LGF::Vector3& playerPos) {
	if (isAttack_) { return; }
	chaHanPosition_ = playerPos;
}

void BaseAnimal::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//進む
	position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_;//ベクトル正規化、スピードに合わせ移動
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
}

void BaseAnimal::Attack(Target target) {	
	if (reverce) {
		t -= 1.0f / 60.0f;
	}
	else {
		t += 1.0f / 60.0f;
	}
	float answer = p1 * t * t * t - p0 * t * t;

	if (target == Target::charhan) {
		position_ += LGF::Math::Normalize(chaHanPosition_ - position_) * answer;
	}
	else if (target == Target::player) {
		position_ += LGF::Math::Normalize(playerPosition_ - position_) * answer;
	}

	if (t >= 1.0f) {
		reverce = true;
	}
	else if (t <= 0.0f) {
		reverce = false;
		isAttack_ = false;
	}
}

void BaseAnimal::Dead() {
	if (rotate_.x >= -1.25f) {
		rotate_.x -= 0.05f;
	}
	deadAnimationTime_ += 1.0f / 60.0f;
}


bool BaseAnimal::IsAttackAreaInTarget(Target target) {
	float dot = 0.0f;
	if (target == Target::charhan) {
		//炒飯との位置関係 //ターゲットと現在位置の差分ベクトル
		//炒飯とのベクトルの大きさ
		dot = LGF::Math::Length(chaHanPosition_ - position_);
	}
	else if (target == Target::player) {
		//プレイヤーとの位置関係 //ターゲットと現在位置の差分ベクトル	
		//炒飯とのベクトルの大きさ
		dot = LGF::Math::Length(playerPosition_ - position_);
	}

	//対象物との距離が一定以下なら進まない
	if (dot <= attackRadius_) {
		isAttack_ = true;
		return true;
	}

	return false;
}
