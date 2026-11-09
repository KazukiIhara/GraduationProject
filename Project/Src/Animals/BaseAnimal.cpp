#include "BaseAnimal.h"

void BaseAnimal::Update() {
	if (!state_) {
		state_ = std::make_unique<MoveAnimalState>();
	}

	state_->SetBaseAnimal(this);
	state_->Update();

	state_ = state_->ChangeState();
}

void BaseAnimal::SetPlayerPosition(const LGF::Vector3& playerPos) {
	playerPosition_ = playerPos;
}

void BaseAnimal::SetCharHanPosition(const LGF::Vector3& playerPos) {
	chaHanPosition_ = playerPos;
}

void BaseAnimal::Attack(Target target) {

	t += (float)LGF::System::DeltaTime();

	if (target == Target::charhan) {
		position_.y = cos(t);
	}
	else if (target == Target::player) {
		position_.y = sin(t);
		//向き
		rotate_.y = std::atan2(LGF::Math::Normalize(playerPosition_ - position_).x, LGF::Math::Normalize(playerPosition_ - position_).z);
	}

	if (t >= 1.0f) {
		t = 0.0f;
		position_.y = 1.0f;
		isAttack_ = false;
	}
}

void BaseAnimal::Dead() {
	if (rotate_.x >= -1.25f) {
		rotate_.x -= 1.25f * (float)LGF::System::DeltaTime();
	}
	deadAnimationTime_ += (float)LGF::System::DeltaTime();
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
