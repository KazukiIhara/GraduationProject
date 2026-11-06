#include "BaseAnimalState.h"
#include "BaseAnimal.h"

#pragma region 移動状態

void MoveAnimalState::Update() {
	animal_->Move();
}

std::unique_ptr<BaseAnimalState> MoveAnimalState::ChangeState() {

	if (animal_->IsAttackAreaInTarget(Target::charhan)) {
		return std::make_unique<AttackCharHanAnimalState>();//炒飯鍋攻撃状態になる
	}
	if (animal_->IsAttackAreaInTarget(Target::player)) {
		return std::make_unique<AttackPlayerAnimalState>();//プレイヤー攻撃状態になる
	}
	if (animal_->IsDead()) {
		return std::make_unique<DeadAnimalState>();//死亡状態
	}
	return std::make_unique<MoveAnimalState>();//変更なし
}

#pragma endregion

#pragma region 炒飯鍋攻撃状態

void AttackCharHanAnimalState::Update() {
	animal_->Attack(Target::charhan);
}

std::unique_ptr<BaseAnimalState> AttackCharHanAnimalState::ChangeState() {
	if (!animal_->IsAttacking() && !animal_->IsAttackAreaInTarget(Target::charhan)) {
		return std::make_unique<MoveAnimalState>();//移動状態に戻る
	}
	if (animal_->IsDead()) {
		return std::make_unique<DeadAnimalState>();//死亡状態
	}
	return std::make_unique<AttackCharHanAnimalState>();//変更なし
}

#pragma endregion

#pragma region プレイヤー攻撃状態

void AttackPlayerAnimalState::Update() {
	animal_->Attack(Target::player);
}

std::unique_ptr<BaseAnimalState> AttackPlayerAnimalState::ChangeState() {
	if (!animal_->IsAttacking() && !animal_->IsAttackAreaInTarget(Target::player)) {
		return std::make_unique<MoveAnimalState>();//移動状態に戻る
	}
	if (animal_->IsDead()) {
		return std::make_unique<DeadAnimalState>();//死亡状態
	}
	return std::make_unique<AttackPlayerAnimalState>();//変更なし
}

#pragma endregion

#pragma region 死亡状態

void DeadAnimalState::Update() {
	animal_->Dead();
}

std::unique_ptr<BaseAnimalState> DeadAnimalState::ChangeState() {
	return std::make_unique<DeadAnimalState>();//変更なし
}

#pragma endregion