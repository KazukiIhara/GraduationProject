#pragma once

#include "LGF/Math/Vector3.h"
#include <algorithm>

// =========================================================
// 火元（中華レンジ・コンロ）管理クラス
// =========================================================
class HeatSource
{
public:
	/// <summary>
	/// 火力レベル
	/// </summary>
	enum class FlameLevel
	{
		Off,     // 消火
		Low,     // 弱火
		Medium,  // 中火
		High,    // 強火
	};

	HeatSource() = default;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="position">火元の中心位置</param>
	/// <param name="radius">熱が届く水平半径</param>
	/// <param name="heightRange">熱が届く高さの許容範囲</param>
	explicit HeatSource(
		const LGF::Vector3& position,
		float radius = 1.2f,
		float heightRange = 1.8f
	) : position_(position), radius_(radius), heightRange_(heightRange) {}

	// -----------------------------------------------------
	// 判定処理
	// -----------------------------------------------------

	/// <summary>
	/// 鍋などの調理器具が火元の加熱範囲内にあるかどうかを判定
	/// </summary>
	bool IsOverHeat(const LGF::Vector3& utensilPosition) const
	{
		if (flameLevel_ == FlameLevel::Off) {
			return false;
		}

		// 水平距離 (XZ)
		float dx = utensilPosition.x - position_.x;
		float dz = utensilPosition.z - position_.z;
		float distSq = dx * dx + dz * dz;

		if (distSq > radius_ * radius_) {
			return false;
		}

		// 垂直距離 (Y): 火元より上かつ許容範囲内
		float dy = utensilPosition.y - position_.y;
		return (dy >= -0.2f && dy <= heightRange_);
	}

	/// <summary>
	/// 火元からの熱出力（℃ / 秒 の上昇ポテンシャル）
	/// </summary>
	float GetHeatOutput() const
	{
		switch (flameLevel_)
		{
		case FlameLevel::Low:
			return 25.0f; // 弱火
		case FlameLevel::Medium:
			return 50.0f; // 中火
		case FlameLevel::High:
			return 90.0f; // 強火
		case FlameLevel::Off:
		default:
			return 0.0f;
		}
	}

	/// <summary>
	/// 到達目標上限温度（℃）
	/// </summary>
	float GetTargetMaxTemperature() const
	{
		switch (flameLevel_)
		{
		case FlameLevel::Low:
			return 120.0f;
		case FlameLevel::Medium:
			return 190.0f;
		case FlameLevel::High:
			return 280.0f;
		case FlameLevel::Off:
		default:
			return 25.0f; // 室温
		}
	}

	// -----------------------------------------------------
	// ゲッター・セッター
	// -----------------------------------------------------

	FlameLevel GetFlameLevel() const { return flameLevel_; }
	void SetFlameLevel(FlameLevel level) { flameLevel_ = level; }

	const LGF::Vector3& GetPosition() const { return position_; }
	void SetPosition(const LGF::Vector3& pos) { position_ = pos; }

	float GetRadius() const { return radius_; }
	void SetRadius(float r) { radius_ = r; }

	bool IsActive() const { return flameLevel_ != FlameLevel::Off; }

private:
	LGF::Vector3 position_ = { 0.0f, 1.0f, 0.0f }; // 中華レンジの天板・バーナー位置
	float radius_ = 1.2f;                           // 水平有効半径
	float heightRange_ = 2.0f;                      // 上方向の有効高さ
	FlameLevel flameLevel_ = FlameLevel::High;      // デフォルトは強火
};
