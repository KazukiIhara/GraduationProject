#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// =========================================================
// 炒飯パラメータ & 食材定義
// =========================================================

/// <summary>
/// 食材の種類
/// </summary>
enum class IngredientType
{
	Rice,        // ご飯（ベース）
	Egg,         // 卵
	Pork,        // 豚肉（狩猟で入手）
	Chicken,     // 鶏肉（狩猟で入手）
	Beef,        // 牛肉（狩猟で入手）
	GreenOnion,  // ネギ
	Seasoning,   // 調味料（塩コショウ・醤油など）
};

/// <summary>
/// 具材情報
/// </summary>
struct Ingredient
{
	IngredientType type = IngredientType::Rice;
	std::string name = "ご飯";
	float volumeBonus = 20.0f;     // ボリューム上昇量
	float umamiBonus = 10.0f;      // 旨味上昇量
	float aromaBonus = 5.0f;       // 香り上昇量
	float moistureBonus = 15.0f;   // 水分量（炒める前のしっとり感）
};

/// <summary>
/// 炒飯の5大パラメータおよび調理状態
/// </summary>
struct CookingParameters
{
	// -----------------------------------------------------
	// スライド12記載の5大パラメータ (0.0f ～ 100.0f)
	// -----------------------------------------------------

	float fluffiness = 10.0f;    // パラパラ度（鍋振り・適切な水分蒸発でUP）
	float aroma = 0.0f;          // 香り（加熱・炒めでUP、動物を誘引）
	float volume = 30.0f;        // ボリューム（ご飯＋具材でUP）
	float temperature = 25.0f;   // 温度（℃。火元で加熱され、離れると低下）
	float umami = 15.0f;         // 旨味（具材投入やおたまでの炒め合わせでUP）

	// -----------------------------------------------------
	// 調理シミュレーション用補助パラメータ
	// -----------------------------------------------------

	float moisture = 60.0f;      // 水分量（高いとベチャベチャ、適度でパラパラ、ゼロになると焦げやすい）
	float burnt = 0.0f;          // 焦げ度（高温で放置するとUP、減点対象）

	int shakeCount = 0;          // 鍋振り回数
	int stirCount = 0;           // おたま炒め回数
	std::vector<IngredientType> addedIngredients; // 投入された具材履歴

	// -----------------------------------------------------
	// クランプ処理
	// -----------------------------------------------------
	void ClampAll()
	{
		fluffiness = std::clamp(fluffiness, 0.0f, 100.0f);
		aroma = std::clamp(aroma, 0.0f, 100.0f);
		volume = std::clamp(volume, 0.0f, 100.0f);
		temperature = std::clamp(temperature, 0.0f, 300.0f);
		umami = std::clamp(umami, 0.0f, 100.0f);
		moisture = std::clamp(moisture, 0.0f, 100.0f);
		burnt = std::clamp(burnt, 0.0f, 100.0f);
	}

	/// <summary>
	/// レーダーチャート用の正規化値 (0.0f ～ 1.0f) を取得
	/// </summary>
	struct NormalizedRadarValues
	{
		float fluffiness; // パラパラ度
		float aroma;      // 香り
		float volume;     // ボリューム
		float temperature;// 温度 (適温 140～200℃ を1.0とする)
		float umami;      // 旨味
	};

	NormalizedRadarValues GetNormalizedValues() const
	{
		NormalizedRadarValues norm{};
		norm.fluffiness = fluffiness / 100.0f;
		norm.aroma = aroma / 100.0f;
		norm.volume = volume / 100.0f;
		norm.umami = umami / 100.0f;

		// 温度は適温（約150～220℃）で最大値1.0
		if (temperature < 60.0f) {
			norm.temperature = temperature / 120.0f; // 冷めている
		} else if (temperature <= 220.0f) {
			norm.temperature = 0.5f + (temperature - 60.0f) / 160.0f * 0.5f; // 適温
		} else {
			norm.temperature = std::max(0.2f, 1.0f - (temperature - 220.0f) / 80.0f * 0.6f); // 熱すぎ
		}

		return norm;
	}

	// -----------------------------------------------------
	// 評論家（JudgeMan）の評価判定
	// -----------------------------------------------------

	enum class JudgeGrade
	{
		Masterpiece, // 究極の炒飯（Sランク）
		Delicious,   // 合格・美味（Aランク）
		Acceptable,  // ギリギリ認める（Bランク）
		Unacceptable // 認められない！（Cランク・不合格）
	};

	struct JudgeEvaluation
	{
		JudgeGrade grade;
		int totalScore;             // 100点満点
		std::string comment;        // 評論家のセリフ
		bool isAccepted;            // 合格かどうか
	};

	JudgeEvaluation Evaluate() const
	{
		JudgeEvaluation eval{};

		// 評価対象パラメータ: パラパラ度(40%)、旨味(35%)、ボリューム(25%)
		// ※ 温度や香りは調理・狩猟誘引用のパラメータであり、最終評価には使用しない
		float score = (fluffiness * 0.40f) + (umami * 0.35f) + (volume * 0.25f);

		// 焦げペナルティ
		if (burnt > 20.0f) {
			score -= (burnt - 20.0f) * 1.5f;
		}

		eval.totalScore = std::clamp(static_cast<int>(std::round(score)), 0, 100);

		// 判定
		if (burnt >= 60.0f) {
			eval.grade = JudgeGrade::Unacceptable;
			eval.isAccepted = false;
			eval.comment = "丸焦げじゃないか！こんな炒飯、認められるか！";
		} else if (eval.totalScore >= 85) {
			eval.grade = JudgeGrade::Masterpiece;
			eval.isAccepted = true;
			eval.comment = "こ、これは究極の炒飯…！やるじゃないか…！";
		} else if (eval.totalScore >= 65) {
			eval.grade = JudgeGrade::Delicious;
			eval.isAccepted = true;
			eval.comment = "パラパラで旨味も十分、なかなかの腕前だな！";
		} else if (eval.totalScore >= 45) {
			eval.grade = JudgeGrade::Acceptable;
			eval.isAccepted = true;
			eval.comment = "ふむ、悪くはないが…まだ改善の余地があるな。";
		} else {
			eval.grade = JudgeGrade::Unacceptable;
			eval.isAccepted = false;
			eval.comment = "こんな炒飯、認められるか！出直してこい！";
		}

		return eval;
	}
};
