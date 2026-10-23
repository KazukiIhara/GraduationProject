#pragma once
#include <memory>
class BaseAnimal;

/// <summary>
/// 動物の状態(基盤クラス)
/// </summary>
class BaseAnimalState {
public:

	void SetBaseAnimal(BaseAnimal* animal) {
		animal_ = animal;
	}

	virtual void Update() = 0;


	virtual std::unique_ptr<BaseAnimalState> ChangeState() = 0;



protected:

	BaseAnimal* animal_;

};

/// <summary>
/// 移動状態(派生クラス)
/// </summary>
class MoveAnimalState final : public BaseAnimalState {
public:

	void Update() override;

	std::unique_ptr<BaseAnimalState> ChangeState() override;

};

/// <summary>
/// 炒飯に攻撃する状態(派生クラス)
/// </summary>
class AttackCharHanAnimalState final : public BaseAnimalState {
public:

	void Update() override;

	std::unique_ptr<BaseAnimalState> ChangeState() override;
};

/// <summary>
/// プレイヤーに攻撃する状態(派生クラス)
/// </summary>
class AttackPlayerAnimalState final : public BaseAnimalState {
public:

	void Update() override;

	std::unique_ptr<BaseAnimalState> ChangeState() override;
};

/// <summary>
/// 死亡状態(派生クラス)
/// </summary>
class DeadAnimalState final : public BaseAnimalState {
public:

	void Update() override;

	std::unique_ptr<BaseAnimalState> ChangeState() override;
};
