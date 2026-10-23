#include "BaseAnimalState.h"
#include "BaseAnimal.h"

void MoveAnimalState::Update() {
	animal_->Move();
}

std::unique_ptr<BaseAnimalState> MoveAnimalState::ChangeState() {

	if (animal_->IsAttackAreaInTarget(Target::charhan)) {
		return std::make_unique<AttackCharHanAnimalState>();
	}
	if (animal_->IsAttackAreaInTarget(Target::player)) {
		return std::make_unique<AttackPlayerAnimalState>();
	}
	if (animal_->IsDead()) {
		return std::make_unique<DeadAnimalState>();
	}
	return nullptr;
}


void AttackCharHanAnimalState::Update() {
	animal_->Attack(Target::charhan);
}

std::unique_ptr<BaseAnimalState> AttackCharHanAnimalState::ChangeState() {
	if (!animal_->IsAttackAreaInTarget(Target::charhan) && !animal_->IsAttacking()) {
		return std::make_unique<MoveAnimalState>();
	}
	if (animal_->IsDead()) {
		return std::make_unique<DeadAnimalState>();
	}
	return nullptr;
}

void AttackPlayerAnimalState::Update() {
	animal_->Attack(Target::player);
}

std::unique_ptr<BaseAnimalState> AttackPlayerAnimalState::ChangeState() {
	if (!animal_->IsAttackAreaInTarget(Target::player) && !animal_->IsAttacking()) {
		return std::make_unique<MoveAnimalState>();
	}
	if (animal_->IsDead()) {
		return std::make_unique<DeadAnimalState>();
	}
	return nullptr;
}

void DeadAnimalState::Update() {
	animal_->Dead();
}

std::unique_ptr<BaseAnimalState> DeadAnimalState::ChangeState() {
	return nullptr;//変更なし
}
