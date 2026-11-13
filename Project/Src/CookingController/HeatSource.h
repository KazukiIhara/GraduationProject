#pragma once

#include "LGF/Math/Vector3.h"

// =========================================================
// 火元（中華レンジ・コンロ）管理クラス
// 中華レンジは常に豪火（強火）。火加減の概念はなく、
// 点火/消火と「調理場に鍋がセットされているか」で加熱を管理する
// =========================================================
class HeatSource
{
public:
	HeatSource() = default;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="position">火元の中心位置（中華レンジの天板）</param>
	explicit HeatSource(const LGF::Vector3& position)
		: position_(position) {}

	// -----------------------------------------------------
	// 熱出力取得
	// -----------------------------------------------------

	/// <summary>
	/// 火元からの熱出力（℃ / 秒 の上昇ポテンシャル）
	/// </summary>
	float GetHeatOutput() const
	{
		return isActive_ ? heatOutput_ : 0.0f;
	}

	/// <summary>
	/// 到達目標上限温度（℃）
	/// </summary>
	float GetTargetMaxTemperature() const
	{
		return isActive_ ? targetMaxTemperature_ : 25.0f; // 室温
	}

	// -----------------------------------------------------
	// ゲッター・セッター
	// -----------------------------------------------------

	bool IsActive() const { return isActive_; }
	void SetActive(bool active) { isActive_ = active; }
	void Ignite() { isActive_ = true; }       // 点火
	void Extinguish() { isActive_ = false; }  // 消火

	const LGF::Vector3& GetPosition() const { return position_; }
	void SetPosition(const LGF::Vector3& pos) { position_ = pos; }

	float GetHeatOutputValue() const { return heatOutput_; }
	void SetHeatOutputValue(float output) { heatOutput_ = output; }

private:
	LGF::Vector3 position_ = { 0.0f, 1.0f, 0.0f }; // 中華レンジの天板・バーナー位置
	bool isActive_ = true;                          // 点火中かどうか（常時ONの火）

	float heatOutput_ = 24.0f;                      // 熱出力（中華レンジの熱 : 24℃/s）
	float targetMaxTemperature_ = 240.0f;           // 上限温度（中華レンジの最高温度 : 240℃）
};
