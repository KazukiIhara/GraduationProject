#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"

// =========================================================
// おたまクラス（右Joy-Con操作）
// =========================================================
class Ladle
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Ladle();

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

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// ジョイコン入力操作
	/// </summary>
	void JoyConInput();

private:
	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 0.15f, 3.0f, -0.15f };
	const LGF::Vector3 kBaseRotation = { -1.35f, 0.0f, -0.25f, };

	// 動作用パラメーター
	const float kMaxForwardLimit = 0.4f; // 前方向の移動限界
	const float kMaxBackwardLimit = -0.4f; // 後方向の移動限界

	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{ "../Assets/Models/ladle.gltf" }; // モデル
	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転

	// 動作用パラメーター
	float currentSwingOffsetZ_ = 0.0f; // 前後のスイングによるオフセット
	float sensitivity_ = 2.0f; // 入力の感度
};

