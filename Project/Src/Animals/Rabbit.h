#pragma once
#include "BaseAnimal.h"

class Rabbit final : public BaseAnimal {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;

private:

	LGF::Vector3 position_{ 0.0f,1.0f,20.0f };

	const float kMoveSpeed_ = 1.0f;

	const float kAttackRadius_ = 5.0f;//範囲に入ったら攻撃する

};

