#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"
#include "LGF/Collision/ColliderWorld.h"
#include "LGF/Physics.h"

// =========================================================
// プレイヤークラス
// =========================================================
class Player {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	Player(LGF::Physics::PhysicsWorld& physicsWorld);
	~Player();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(float cameraYaw);

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
	/// 位置を取得
	/// </summary>
	/// <returns></returns>
	const LGF::Vector3& GetPosition() const { return position_; }

	/// <summary>
	/// 回転を取得
	/// </summary>
	/// <returns></returns>
	const LGF::Vector3& GetRotation() const { return rotation_; }

	/// <summary>
	/// カメラの視点用オフセット
	/// </summary>
	/// <returns></returns>
	LGF::Vector3 GetEyePosition() const { return position_ + LGF::Vector3{0.0f, 2.8f, 0.0f}; };

	/// <summary>
	/// 料理モード中かどうかを取得
	/// </summary>
	/// <returns></returns>
	bool IsCookingMode() const { return isCookingMode_; }

	/// <summary>
	/// 調理モードに入った瞬間を検知
	/// </summary>
	/// <returns></returns>
	bool JustEnteredCookingMode() const { return justEnteredCookingMode_; }

	/// <summary>
	/// 左手部分のワールド位置を取得（中華鍋の位置設定に使用）
	/// </summary>
	/// <returns></returns>
	LGF::Vector3 GetLeftHandWorldPosition(float cameraPitch = 0.0f) const;

	/// <summary>
	/// 右手部分のワールド位置を取得（おたまの位置設定に使用）
	/// </summary>
	/// <returns></returns>
	LGF::Vector3 GetRightHandWorldPosition(float cameraPitch = 0.0f) const;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 入力処理（※現在はキーボードだが、将来的にJoy-Conに変更する）
	/// </summary>
	/// <param name="outMoveDir"></param>
	/// <returns></returns>
	bool ProcessInput(LGF::Vector3& outMoveDir);

private:
	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kInitPosition = {0.0f, 2.0f, -10.0f}; // 初期位置
	const float kMoveSpeed = 8.0f; // 移動速度
	const float kDeadZone = 0.15f; // スティックのデッドゾーン

	const LGF::Vector3 kColliderMin = {-1.0f, -2.0f, -1.0f};
	const LGF::Vector3 kColliderMax = {1.0f, 2.0f, 1.0f};

	const float kSphereRadius = 1.0f;
	const LGF::Vector3 kSphereOffset = {0.0f, -1.0f, 0.0f}; // 球体オフセット

	const LGF::Vector3 kCookingWarpPosition = {0.0f, 2.0f, -2.0f}; // 調理モード時の固定位置
	const LGF::Vector3 kLeftHandOffset = {-1.1f, 1.0f, 0.8f}; // 左手オフセット（中華鍋基準位置として使用）
	const LGF::Vector3 kRightHandOffset = {0.7f, 1.4f, 1.7f}; // 右手オフセット（おたま基準位置として使用）

	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{"../Assets/Models/player.gltf"}; // モデル（可視化デバッグ用）
	LGF::Vector3 position_{0.0f, 0.0f, 0.0f}; // 位置
	LGF::Vector3 rotation_{0.0f, 0.0f, 0.0f}; // 回転

	LGF::Collision::ColliderHandle colliderHandle_; // コライダーハンドル

	LGF::Physics::PhysicsWorld& physicsWorld_; // 物理ワールド
	LGF::Physics::DynamicObjectHandle physicsHandle_; // 物理ハンドル

	bool isNearChineseRange_ = false; // 中華レンジの範囲内にいるか
	bool isCookingMode_ = false; // 料理モード中かどうか（移動不可・視点移動不可）
	bool justEnteredCookingMode_ = false; // 調理モードに入った瞬間の判定
};
