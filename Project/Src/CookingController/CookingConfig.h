#pragma once

#include "CookingParameters.h"
#include <filesystem>
#include <string>
#include <unordered_map>

// =========================================================
// 調理システムのチューニングパラメータ設定
// 外部ファイル（Assets/Parameters/CookingConfig.json）との同期に対応
// =========================================================
struct CookingTuningConfig
{
	// -----------------------------------------------------
	// 1. 熱・温度
	// -----------------------------------------------------
	float heatOutput = 24.0f;               // 中華レンジの熱出力 (℃/s)
	float targetMaxTemperature = 240.0f;    // 中華レンジの到達最高温度 (℃)
	float naturalCoolingRate = 2.5f;        // 自然放熱係数 (℃/s)
	float roomTemperature = 25.0f;          // 室温・下限温度 (℃)
	float tempDiffCoolingCoeff = 0.015f;    // 温度差による自然冷却加速係数

	// -----------------------------------------------------
	// 2. 水分蒸発・焦げ
	// -----------------------------------------------------
	float evapRateCoeff = 0.22f;            // 水分蒸発の基準係数 (100℃以上のsqrtスケーリング)
	float residualEvapMultiplier = 0.15f;   // 余熱時の蒸発倍率 (加熱中の15%)
	float burnStartTemp = 180.0f;           // 焦げ始める温度 (℃)
	float burnMaxMoisture = 12.0f;          // 焦げ始める水分上限 (%)
	float burnGraceTime = 5.0f;             // 振らずに放置したときの焦げ猶予時間 (秒)
	float burnRateCoeff = 1.0f;             // 焦げ進行速度係数

	// -----------------------------------------------------
	// 3. 鍋振りアクション効果 (縦あおり / 横回し)
	// -----------------------------------------------------
	float shakeFluffinessVertical = 2.0f;   // 縦振りのパラパラ上昇値
	float shakeAromaVertical = 2.5f;        // 縦振りの香り拡散値
	float shakeMoistureLossVertical = 0.3f; // 縦振りの水分蒸発ボーナス
	float shakeCoolingVertical = 12.0f;     // 縦振りの冷却値 (℃)
	float shakeUmamiHorizontal = 1.2f;      // 横振りの旨味上昇値
	float shakeFluffinessHorizontal = 0.8f; // 横振りのパラパラ上昇値
	float shakeCoolingHorizontal = 3.5f;    // 横振りの冷却値 (℃)

	// 黄金ゾーン（パラパラ効率）
	float goldenZoneTempMin = 120.0f;       // 黄金ゾーン最低温度 (℃)
	float goldenZoneMoistMin = 10.0f;       // 黄金ゾーン最低水分 (%)
	float goldenZoneMoistMax = 35.0f;       // 黄金ゾーン最高水分 (%)
	float goldenZoneEfficiency = 1.3f;      // 黄金ゾーン倍率
	float wetZoneEfficiency = 0.6f;         // ベチャベチャ時倍率
	float dryZoneEfficiency = 0.2f;         // パサパサ時倍率

	// 強さの閾値倍率
	float strengthWeakCoeff = 0.5f;         // 弱の係数
	float strengthMediumCoeff = 1.0f;       // 中の係数
	float strengthStrongCoeff = 1.5f;       // 強の係数

	// -----------------------------------------------------
	// 4. おたまアクション効果
	// -----------------------------------------------------
	float ladleUmamiRate = 1.0f;            // おたま炒めの旨味上昇率 (毎秒)
	float ladleFluffinessRate = 0.5f;       // おたま炒めのパラパラ上昇率 (毎秒)
	float ladleStirUmamiInstant = 0.8f;     // おたま単発炒めの旨味
	float ladleStirFluffinessInstant = 0.4f;// おたま単発炒めのパラパラ
	float ladleSwingSpeedScale = 1.5f;      // スイング速度換算係数
	float ladleSwingThreshold = 0.3f;       // スイング炒め判定閾値

	// -----------------------------------------------------
	// 5. 香り・動物出現レベル（等間隔5段階）
	// -----------------------------------------------------
	float aromaDecayRate = 0.2f;            // 香りの毎秒自然減衰値
	float aromaLevelThreshold1 = 20.0f;     // Lv1 -> Lv2 閾値 (デフォルト: 20)
	float aromaLevelThreshold2 = 40.0f;     // Lv2 -> Lv3 閾値 (デフォルト: 40)
	float aromaLevelThreshold3 = 60.0f;     // Lv3 -> Lv4 閾値 (デフォルト: 60)
	float aromaLevelThreshold4 = 80.0f;     // Lv4 -> Lv5 閾値 (デフォルト: 80)

	// -----------------------------------------------------
	// 6. 初期状態パラメータ
	// -----------------------------------------------------
	float initFluffiness = 10.0f;
	float initAroma = 0.0f;
	float initVolume = 30.0f;
	float initTemperature = 25.0f;
	float initUmami = 15.0f;
	float initMoisture = 55.0f;

	// -----------------------------------------------------
	// 7. 食材投入ボーナス（動的マップ管理：新規食材追加に対応）
	// -----------------------------------------------------
	struct IngredientBonus
	{
		std::string displayName; // 表示名
		float volume = 15.0f;    // ボリューム上昇量
		float umami = 10.0f;     // 旨味上昇量
		float aroma = 5.0f;      // 香り上昇量
		float moisture = 10.0f;  // 水分量
	};

	// 食材ごとのパラメータテーブル（キー: "Rice", "Egg", "GreenOnion", "Meat" 等）
	std::unordered_map<std::string, IngredientBonus> ingredientBonuses;

	/// <summary>
	/// 食材のボーナスを取得（JSONに存在しない食材でも安全にデフォルト値を返却）
	/// </summary>
	IngredientBonus GetIngredientBonus(const std::string& name) const;

	/// <summary>
	/// 食材のデフォルト設定値を取得
	/// </summary>
	static IngredientBonus GetDefaultIngredientBonus(const std::string& name);

	// -----------------------------------------------------
	// 8. 評論家審査・スコア配分
	// -----------------------------------------------------
	float targetRiceAmount = 150.0f;        // 満点（volume = 100%）となる目標米量（g）
	JudgeConfig judge;

	// -----------------------------------------------------
	// 外部ファイル読み書き
	// -----------------------------------------------------
	bool LoadFromFile(const std::filesystem::path& path = "../Assets/Parameters/CookingConfig.json");
	bool SaveToFile(const std::filesystem::path& path = "../Assets/Parameters/CookingConfig.json") const;
	void ResetToDefault();
};
