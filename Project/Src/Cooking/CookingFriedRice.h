#pragma once

#include <vector>

// =========================================================
// 食材の種類
// =========================================================
enum class IngredientType
{
	Egg,
	GreenOnion,
	Meat,
};

// =========================================================
// 炒飯の調理状態
// =========================================================
enum class FriedRiceState
{
	NotStarted,
	Cooking,
	Completed,
	Burned,
};

// =========================================================
// 投入された食材の情報
// =========================================================
struct IngredientData
{
	IngredientType type{};
	float amount = 0.0f;
};

// =========================================================
// 調理中の炒飯クラス
// =========================================================
class CookingFriedRice
{
public:
	/// <summary>
	/// 米を追加
	/// </summary>
	/// <param name="amount">追加する米の量（g）</param>
	void AddRice(float amount);

	/// <summary>
	/// 食材を追加
	/// </summary>
	void AddIngredient(IngredientType type, float amount);

	/// <summary>
	/// 現在の米の量を取得
	/// </summary>
	float GetRiceAmount() const;

	/// <summary>
	/// 投入された食材を取得
	/// </summary>
	const std::vector<IngredientData>& GetIngredients() const;

	/// <summary>
	/// 現在の調理状態を取得
	/// </summary>
	FriedRiceState GetState() const;

	/// <summary>
	/// 調理状態を設定
	/// </summary>
	void SetState(FriedRiceState state);

private:
	float riceAmount_ = 0.0f; // 現在の米の量（g）
	std::vector<IngredientData> ingredients_; // 投入された食材
	FriedRiceState state_ = FriedRiceState::NotStarted; // 現在の調理状態
};
