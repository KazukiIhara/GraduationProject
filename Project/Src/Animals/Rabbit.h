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

	/// <summary>
	/// 移動方法
	/// </summary>
	void Move() override;

private:

	const float kJumpCurvePointY_ = 5.0f;

	float jumpTimer_ = 0.0f;
	const float kJumpMoveFrequency_ = 2.0f;


	LGF::Vector3 startPoint;

	LGF::Vector3 halfPoint;

	LGF::Vector3 endPoint;
};

