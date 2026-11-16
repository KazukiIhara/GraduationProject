#pragma once

#include <string>
#include <vector>

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
/// 鍋振りの向き
/// </summary>
enum class WokShakeDirection
{
	None,
	Vertical,   // 縦振り（あおり・浮かし）：宙に舞い、パラパラ度UP・水分蒸発・香り拡散
	Horizontal, // 横振り（水平回し・鍋肌）：鍋肌で全体を均一化、旨味UP・焦げ防止
};

/// <summary>
/// 鍋振りの強さレベル（閾値段階）
/// </summary>
enum class ShakeStrengthLevel
{
	None,
	Weak,   // 弱
	Medium, // 中
	Strong, // 強
};

/// <summary>
/// 評論家審査パラメータ（配点ウェイト・減点閾値）
/// </summary>
struct JudgeConfig
{
	float fluffinessWeight = 0.40f;    // パラパラ度配分 (40%)
	float umamiWeight = 0.35f;         // 旨味配分 (35%)
	float volumeWeight = 0.25f;        // ボリューム配分 (25%)
	float burnPenaltyThreshold = 20.0f;// 焦げ減点開始閾値 (%)
	float burnPenaltyRate = 1.5f;      // 焦げ減点レート
	float burntFailThreshold = 60.0f;  // 即不合格の焦げ閾値 (%)
	int scoreMasterpiece = 85;         // Sランク点数
	int scoreDelicious = 65;           // Aランク点数
	int scoreAcceptable = 45;          // Bランク点数
};

/// <summary>
/// 炒飯の5大パラメータおよび調理状態
/// </summary>
struct CookingParameters
{
	// -----------------------------------------------------
	// 5大パラメータ (0.0f ～ 100.0f)
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

	int shakeCount = 0;          // 鍋振り総回数
	int verticalShakeCount = 0;  // 縦振り（あおり）回数
	int horizontalShakeCount = 0;// 横振り（水平回し）回数
	int stirCount = 0;           // おたま炒め回数
	std::vector<IngredientType> addedIngredients; // 投入された具材履歴

	// -----------------------------------------------------
	// パラメータクランプ
	// -----------------------------------------------------
	void ClampAll();

	/// <summary>
	/// レーダーチャート用の正規化値 (0.0f ～ 1.0f)
	/// スライド12「究極の炒飯を作ろう」の5大パラメータに対応
	/// </summary>
	struct RadarChartData
	{
		float fluffiness = 0.0f; // パラパラ度 (0.0f ～ 1.0f)
		float aroma = 0.0f;      // 香り (0.0f ～ 1.0f)
		float volume = 0.0f;     // ボリューム (0.0f ～ 1.0f)
		float temperature = 0.0f;// 温度適正度 (0.0f ～ 1.0f: 適温 150～210℃ で最大1.0)
		float umami = 0.0f;      // 旨味 (0.0f ～ 1.0f)

		/// <summary>
		/// 5角形の平均スコア (0.0f ～ 1.0f)
		/// </summary>
		float GetAverage() const
		{
			return (fluffiness + aroma + volume + temperature + umami) / 5.0f;
		}
	};

	/// <summary>
	/// レーダーチャート用正規化データを取得
	/// </summary>
	RadarChartData GetRadarChartData() const;

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
		JudgeGrade grade = JudgeGrade::Unacceptable;
		int totalScore = 0;                     // 100点満点
		std::string comment;                    // 評論家のセリフ（日本語・会話UI用）
		std::string commentEn;                  // 評論家のセリフ（英語・ImGui用）
		bool isAccepted = false;                // 合格かどうか
		RadarChartData radarChart;              // 5大パラメータレーダーチャート
		std::vector<std::string> messageTextures;// 会話表示用テクスチャ名リスト
	};

	/// <summary>
	/// 評論家による炒飯の総合評価
	/// </summary>
	JudgeEvaluation Evaluate(const JudgeConfig& config = JudgeConfig{}) const;
};
