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
	/// 描画処理
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 移動方法
	/// </summary>
	void Move() override;

private:
	//線形補間の中間のY軸を上にする
	const float kJumpCurvePointY_ = 5.0f;
	
	//ジャンプ時間
	float jumpTimer_ = 0.0f;
	const float kJumpTimeFinish_ = 1.0f;
	//ジャンプ猶予
	float jumpDelayTimer_ = 0.0f;
	const float kJumpDelayTimeFinish_ = 0.25f;

	//線形補間
	LGF::Vector3 startPoint;//始点
	LGF::Vector3 halfPoint;//中間
	LGF::Vector3 endPoint;//終点
};

