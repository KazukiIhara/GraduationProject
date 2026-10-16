#pragma once
#include "BaseAnimal.h"

/// <summary>
/// 動物　-ウサギ-
/// </summary>
class Rabbit final : public BaseAnimal {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(const LGF::Vector3& spownPos)override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;

private:

	const float kAttackRadius_ = 5.0f;//範囲に入ったら攻撃する

};

