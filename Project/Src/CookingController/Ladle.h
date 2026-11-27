#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"

#include "Collision/CollisionManager.h"

// =========================================================
// おたまクラス（右Joy-Con操作）
// =========================================================
class Ladle
{
public:
	enum class AttackType
	{
		Horizontal, // 横振り
		Vertical, // 縦振り
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	Ladle();
	~Ladle();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const;

	/// <summary>
	/// ImGuiデバッグ描画
	/// </summary>
	void DrawImGuiDebug();

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// おたまの位置を取得
	/// </summary>
	const LGF::Vector3& GetPosition() const { return position_; }

	/// <summary>
	/// おたまの回転を取得
	/// </summary>
	const LGF::Vector3& GetRotation() const { return rotation_; }

	/// <summary>
	/// 前後スイングの現在のオフセット値を取得 (-kMaxBackwardLimit ～ kMaxForwardLimit)
	/// </summary>
	float GetCurrentSwingOffsetZ() const { return currentSwingOffsetZ_; }

	/// <summary>
	/// 前後スイングの正規化された強度 (-1.0f ～ 1.0f)
	/// </summary>
	float GetSwingNormalized() const
	{
		return (kMaxForwardLimit > 0.0f) ? (currentSwingOffsetZ_ / kMaxForwardLimit) : 0.0f;
	}

	/// <summary>
	/// ベース位置の設定
	/// </summary>
	/// <param name="basePos"></param>
	/// <param name="baseRot"></param>
	/// <param name="snapImmediate"></param>
	void SetBaseTransform(const LGF::Vector3& basePos, const LGF::Vector3& baseRot, bool snapImmediate = false);

	/// <summary>
	/// 鍋振り許可フラグの設定
	/// </summary>
	/// <param name="canShake"></param>
	void SetCanAttack(bool canAttack) { canAttack_ = canAttack; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// コントローラー入力操作
	/// </summary>
	void ControllerInput();

	/// <summary>
	/// 攻撃入力捜査
	/// </summary>
	void AttackInput();

	/// <summary>
	/// 攻撃処理更新
	/// </summary>
	/// <param name="dt"></param>
	void UpdateAttack(float dt);

	/// <summary>
	/// 先端のワールド座標を取得（先端コライダー用）
	/// </summary>
	/// <returns></returns>
	LGF::Vector3 GetTipWorldPosition() const;

private:
	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 0.15f, 3.0f, -0.15f };
	const LGF::Vector3 kBaseRotation = { -1.35f, 0.0f, -0.25f, };

	// 動作用パラメーター
	const float kMaxForwardLimit = 0.4f; // 前方向の移動限界
	const float kMaxBackwardLimit = -0.4f; // 後方向の移動限界

	// おたまの先端コライダー用パラメーター
	const LGF::Vector3 kTipOffset = {0.0f, 0.0f, 0.0f}; // 先端のオフセット
	const float kTipSphereRadius = 0.35f; // 先端の半径

	// 攻撃モーション用パラメーター
	const float kAttackDuration = 0.35f; // 1回あたりの振りモーション（秒）
	const float kAttackInterval = 0.4f; // 攻撃間隔（秒）
	const float kComboResetTime = 2.0f; // 未攻撃で横振りへ戻る時間（秒）

	const LGF::Vector3 kHorizontalMoveOffset = {0.6f, 0.0f, 0.3f};
	const LGF::Vector3 kHorizontalRotOffset = {0.0f, 0.9f, 0.5f};

	const LGF::Vector3 kVerticalMoveOffset = {0.0f, -1.2f, 0.5f};
	const LGF::Vector3 kVerticalRotOffset = {1.4f, 0.0f, 0.0f};

	const float kThrustDistance = 2.0f; // 突き出し距離
	const LGF::Vector3 kThrustRotOffset = {0.25f, 0.0f, 0.0f};
	const float kAttackMotionLerpRate = 0.25f;

	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{ "../Assets/Models/ladle.gltf" }; // モデル
	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転

	LGF::Vector3 basePosition_{0.0f, 0.0f, 0.0f}; // ベース位置
	LGF::Vector3 baseRotation_{0.0f, 0.0f, 0.0f}; // ベース回転

	// 動作用パラメーター
	float currentSwingOffsetZ_ = 0.0f; // 前後のスイングによるオフセット
	float sensitivity_ = 2.0f; // 入力の感度

	// 攻撃用パラメーター
	bool isAttacking_ = false;
	float attackTimer_ = 0.0f;
	float attackCooldownTimer_ = 0.0f;
	float idleTimeSinceLastAttack_ = 0.0f;
	AttackType nextAttackType_ = AttackType::Horizontal;
	AttackType currentAttackType_ = AttackType::Horizontal;

	bool canAttack_ = false; // 攻撃の許可（料理中は不可）
};

