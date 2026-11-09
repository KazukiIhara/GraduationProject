#pragma once

#include "CookingParameters.h"
#include "HeatSource.h"
#include "LGF/Math/Vector3.h"

// =========================================================
// 調理システム（火元・パラメータ・調理処理の統合マネージャー）
// =========================================================
class CookingSystem
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	CookingSystem();
	~CookingSystem() = default;

	/// <summary>
	/// リセット処理（初期状態に戻す）
	/// </summary>
	void Reset();

	/// <summary>
	/// 毎フレームの調理更新処理
	/// </summary>
	/// <param name="deltaTime">フレーム経過時間（秒）</param>
	/// <param name="wokPosition">鍋のワールド座標</param>
	/// <param name="isWokShaking">鍋が振られているか（Wokのアニメーション中など）</param>
	/// <param name="ladleStirStrength">おたまの炒め入力強度 (0.0f～1.0f)</param>
	void Update(
		float deltaTime,
		const LGF::Vector3& wokPosition,
		bool isWokShaking,
		float ladleStirStrength = 0.0f
	);

	// -----------------------------------------------------
	// 調理アクション
	// -----------------------------------------------------

	/// <summary>
	/// 鍋振りアクションの通知（1回振られた瞬間やアニメーション開始時に呼び出す）
	/// </summary>
	/// <param name="shakeIntensity">振りの強さ (1.0fが標準)</param>
	void OnWokShake(float shakeIntensity = 1.0f);

	/// <summary>
	/// おたまで炒めるアクションの通知
	/// </summary>
	/// <param name="stirIntensity">炒めの強さ (1.0fが標準)</param>
	void OnLadleStir(float stirIntensity = 1.0f);

	/// <summary>
	/// 食材を中華鍋に投入
	/// </summary>
	/// <param name="type">投入する食材の種類</param>
	void AddIngredient(IngredientType type);

	/// <summary>
	/// 食材投入（詳細定義指定）
	/// </summary>
	void AddIngredient(const Ingredient& ingredient);

	// -----------------------------------------------------
	// ImGuiデバッグ表示
	// -----------------------------------------------------

	/// <summary>
	/// パラメータや火元状態の確認・手動テスト用UI
	/// </summary>
	void DrawImGuiDebug();

	// -----------------------------------------------------
	// ゲッター・セッター
	// -----------------------------------------------------

	const CookingParameters& GetParameters() const { return parameters_; }
	CookingParameters& GetParameters() { return parameters_; }

	const HeatSource& GetHeatSource() const { return heatSource_; }
	HeatSource& GetHeatSource() { return heatSource_; }

	/// <summary>
	/// 鍋が現在火元の上で加熱されているか
	/// </summary>
	bool IsHeating() const { return isHeating_; }

	/// <summary>
	/// 動物を引き寄せる匂い・香りの強さを取得 (0.0f ～ 1.0f)
	/// </summary>
	float GetAttractAnimalStrength() const { return parameters_.aroma / 100.0f; }

	/// <summary>
	/// 評論家による現在の炒飯の評価を取得
	/// </summary>
	CookingParameters::JudgeEvaluation GetCurrentEvaluation() const
	{
		return parameters_.Evaluate();
	}

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 火元による温度変化・自然冷却の更新
	/// </summary>
	void UpdateHeat(float deltaTime, const LGF::Vector3& wokPosition);

	/// <summary>
	/// 水分の蒸発と焦げの進行更新
	/// </summary>
	void UpdateMoistureAndBurnt(float deltaTime, bool isWokShaking);

	/// <summary>
	/// 香りの拡散・減衰更新
	/// </summary>
	void UpdateAroma(float deltaTime);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	CookingParameters parameters_;
	HeatSource heatSource_;

	bool isHeating_ = false;           // 現在加熱中かどうか
	float heatExposureTimer_ = 0.0f;   // 鍋を振らずに火にかけ続けている時間（焦げ判定用）

	// 常温（室温）
	const float kRoomTemperature = 25.0f;
	// 自然放熱係数
	const float kCoolingRate = 8.0f;
};
