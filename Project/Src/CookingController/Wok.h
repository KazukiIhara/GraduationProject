#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"

#include "../Calibration/Calibration.h"

// =========================================================
// 中華鍋クラス（左手Joy-Con操作）
// =========================================================
class Wok
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Wok();

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
	/// 鍋のワールド座標を取得
	/// </summary>
	const LGF::Vector3& GetPosition() const { return position_; }

	/// <summary>
	/// 鍋の回転を取得
	/// </summary>
	const LGF::Vector3& GetRotation() const { return rotation_; }

	/// <summary>
	/// 鍋振りアニメーション中かどうか
	/// </summary>
	bool IsAnimationPlaying() const { return isAnimationPlaying_; }

	/// <summary>
	/// 鍋振りアニメーションの進行度 (0.0f ～ 1.0f)
	/// </summary>
	float GetAnimationProgress() const
	{
		return animationDuration_ > 0.0f ? std::min(animationTime_ / animationDuration_, 1.0f) : 0.0f;
	}

private:
	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 0.0f, 2.5f, 0.0f };
	const LGF::Vector3 kBaseRotation = { 0.0f, 0.0f, 0.0f };

	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{ "../Assets/Models/wok.gltf" }; // モデル
	LGF::Vector3 position_{0.0f, 0.0f, 0.0f}; // 位置
	LGF::Vector3 rotation_{0.0f, 0.0f, 0.0f}; // 回転

	std::unique_ptr<Calibration> calibration_ = nullptr; // キャリブレーション

	bool isAnimationPlaying_ = false; // アニメーション再生中かどうか
	const float animationDuration_ = 0.5f; // アニメーションの再生時間（秒）
	float animationTime_ = 0.0f; // アニメーションの経過時間（秒）

};

