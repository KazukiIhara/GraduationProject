#pragma once
#include "Animals/Base/BaseAnimal.h"

/// <summary>
/// 動物 -猪-
/// </summary>
class Boar final : public BaseAnimal {
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

	float tackleTime_ = 0.0f;
	const float kTackleTimeMax_ = 2.0f;

	LGF::Vector3 tackleStart_{};
	LGF::Vector3 tackleGoal_{};

};

