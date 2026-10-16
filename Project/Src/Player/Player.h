#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"

// =========================================================
// プレイヤークラス
// =========================================================
class Player {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

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
	// Getter / Setter
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

	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{"../Assets/Models/player.gltf"}; // モデル（可視化デバッグ用）
	LGF::Vector3 position_{0.0f, 0.0f, 0.0f}; // 位置
	LGF::Vector3 rotation_{0.0f, 0.0f, 0.0f}; // 回転
};
