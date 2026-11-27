#pragma once
#include "Animals/Base/BaseAnimal.h"

/// <summary>
/// 動物 -シカ-
/// </summary>
class Deer final : public BaseAnimal {
public:

	/// <summary>
	/// 移動方法
	/// </summary>
	void Move() override;
	/// <summary>
	/// 攻撃処理
	/// </summary>
	/// <param name="target">プレイヤーか炒飯か</param>
	void Attack(Target target) override;

private:
	float attackTime_ = 0.0f;
	const float KAttackTimeMax_ = 1.0f;

	LGF::Vector3 bitePoint_{};
};

