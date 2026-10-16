#pragma once
#include "GameApp.h"

/// <summary>
/// 動物の基盤クラス
/// </summary>
class BaseAnimal {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="spownPos">召喚場所</param>
	virtual void Init(const LGF::Vector3& spownPos) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// Setter_炒飯がある中華鍋の位置
	/// </summary>
	/// <param name="playerPos">現在のプレイヤーの位置を設定</param>
	void SetPlayerPosition(const LGF::Vector3& playerPos);

	/// <summary>
	/// 消滅処理
	/// </summary>
	/// <returns>死んだアニメーションのMax値を超えた</returns>
	bool IsDelete() {
		return deadAnimationTime_ >= kDeadAnimationMaxTime_;
	}

	void Dead();

protected:

	/// <summary>
	/// ターゲットに向かう処理
	/// </summary>
	void GoToTarget();

protected:

	LGF::Box3D object_;

	LGF::Vector3 position_;

	float moveSpeed_ = 0.0f;

	LGF::Vector3 targetPosition_;

	int hp_ = 500;

	float deadAnimationTime_ = 0.0f;
	const float kDeadAnimationMaxTime_ = 2.0f;
};

