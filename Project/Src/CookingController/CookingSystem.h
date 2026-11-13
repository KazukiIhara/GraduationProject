#pragma once

#include "CookingParameters.h"
#include "HeatSource.h"
#include "LGF/Math/Vector3.h"

// =========================================================
// 調理システムのチューニング用パラメータ設定
// =========================================================
struct CookingTuningConfig
{
	// 熱・温度
	float heatOutput = 24.0f;				// 中華レンジの熱出力 (℃/s) - 常温25℃から適温180℃まで約6〜7秒(暫定)
	float coolingRate = 2.5f;				// 自然放熱係数 (℃/s) - 余熱が適度に持続する穏やかな冷却

	// 水分蒸発・焦げ
	float evapRateCoeff = 0.22f;			// 水分蒸発の基準係数（平方根スケーリングで高温時の急増を抑制）
	float residualEvapMultiplier = 0.15f;	// 余熱時の蒸発倍率（加熱中の15%で水分が飛ぶ）
	float burnGraceTime = 3.5f;				// 焦げ始めるまでの猶予時間 (秒)
	float burnRateCoeff = 1.0f;				// 焦げ進行速度係数
	float burnStartTemp = 180.0f;			// 焦げ始める温度 (℃)
	float burnMaxMoisture = 12.0f;			// 焦げ始める水分上限 (%)

	// 鍋振りアクション効果（向き別の効果設定）
	float shakeFluffinessVertical = 2.0f;   // 縦振りのパラパラ上昇値
	float shakeAromaVertical = 2.5f;        // 縦振りの香り拡散値
	float shakeMoistureLossVertical = 0.3f; // 縦振りの水分蒸発ボーナス
	float shakeCoolingVertical = 12.0f;     // 縦振りの冷却値 (℃) - 空中に舞い熱を放出
	float shakeUmamiHorizontal = 1.2f;      // 横振りの旨味上昇値
	float shakeFluffinessHorizontal = 0.8f; // 横振りのパラパラ上昇値
	float shakeCoolingHorizontal = 3.5f;    // 横振りの冷却値 (℃) - 鍋肌を滑る微小冷却

	// 強さの閾値係数（暫定で3段階）
	float strengthWeakCoeff = 0.5f;         // 弱の係数
	float strengthMediumCoeff = 1.0f;       // 中の係数
	float strengthStrongCoeff = 1.5f;       // 強の係数

	// おたまアクション効果
	float ladleUmamiRate = 1.0f;			// おたま炒めの旨味上昇率 (毎秒)
	float ladleFluffinessRate = 0.5f;		// おたま炒めのパラパラ上昇率 (毎秒)

	// 香り減衰
	float aromaDecayRate = 0.2f;			// 香りの毎秒自然減衰値
};

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
	/// 毎フレームの調理更新処理（ステートベース：調理場フラグ・鍋振り・炒め入力で更新）
	/// 調理場にセットされているかで自動加熱/冷却を管理します
	/// </summary>
	void Update(
		float deltaTime,
		bool isAtCookingStation,
		bool isWokShaking,
		float ladleStirStrength = 0.0f
	);

	/// <summary>
	/// 毎フレームの調理更新処理（内部状態とDeltaTimeを使用する簡易オーバーロード）
	/// </summary>
	void Update();

	// -----------------------------------------------------
	// 調理アクション（向き・強さベース）
	// -----------------------------------------------------

	/// <summary>
	/// 鍋振りアクションの通知（向きと強さの閾値段階を指定）
	/// </summary>
	/// <param name="direction">振りの向き（Vertical:縦あおり, Horizontal:横回し）</param>
	/// <param name="strengthLevel">強さの閾値段階（Weak / Medium / Strong）</param>
	void OnWokShake(
		WokShakeDirection direction,
		ShakeStrengthLevel strengthLevel = ShakeStrengthLevel::Medium
	);

	/// <summary>
	/// 鍋振りアクションの通知（向きと連続強度 0.0f～1.0f以上 を指定）
	/// </summary>
	void OnWokShake(WokShakeDirection direction, float intensity);

	/// <summary>
	/// 従来の互換用OnWokShake（デフォルト縦あおり扱い）
	/// </summary>
	/// <param name="shakeIntensity">振りの強さ (1.0fが標準)</param>
	void OnWokShake(float shakeIntensity = 1.0f);

	/// <summary>
	/// おたまで炒めるアクションの通知
	/// </summary>
	/// <param name="stirIntensity">炒めの強さ (1.0fが標準)</param>
	void OnLadleStir(float stirIntensity = 1.0f);

	/// <summary>
	/// おたまの前後スイング移動量から炒め強度を自動反映（Ladle組み込み用）
	/// </summary>
	/// <param name="swingDeltaZ">フレーム間のスイング移動差分</param>
	/// <param name="deltaTime">経過時間</param>
	void ProcessLadleSwing(float swingDeltaZ, float deltaTime);

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
	// 外部（中華レンジ・鍋・動物・審査員）との接続支援
	// -----------------------------------------------------

	/// <summary>
	/// 調理場（コンロ近く）に鍋がセットされているかを設定
	/// true: 鍋がコンロに自動オフセットされ、加熱状態になります
	/// false: コンロから鍋が離れ、加熱が停止（自然放熱）します
	/// </summary>
	void SetAtCookingStation(bool isAtStation) { isAtCookingStation_ = isAtStation; }
	bool IsAtCookingStation() const { return isAtCookingStation_; }

	/// <summary>
	/// 中華レンジ（StageManager）の配置位置を火元位置に設定
	/// </summary>
	void SetHeatSourcePosition(const LGF::Vector3& pos) { heatSource_.SetPosition(pos); }

	/// <summary>
	/// 中華レンジの点火/消火を設定（火加減はなく常に豪火）
	/// </summary>
	void SetStoveActive(bool active) { heatSource_.SetActive(active); }
	void IgniteStove() { heatSource_.Ignite(); }
	void ExtinguishStove() { heatSource_.Extinguish(); }

	/// <summary>
	/// 動物を引き寄せる匂い・香りの強さを取得 (0.0f ～ 1.0f)
	/// </summary>
	float GetAttractAnimalStrength() const { return parameters_.aroma / 100.0f; }

	/// <summary>
	/// 香りが届く最大有効半径（メートル）。BaseAnimalの索敵範囲用
	/// </summary>
	float GetAttractRadius(float maxRadius = 35.0f) const
	{
		return maxRadius * GetAttractAnimalStrength();
	}

	/// <summary>
	/// 指定座標の動物が炒飯の匂い感知範囲に入っているか判定
	/// </summary>
	bool IsAnimalAttracted(const LGF::Vector3& animalPos, float maxRadius = 35.0f) const;

	/// <summary>
	/// 評論家による現在の炒飯の評価を取得
	/// </summary>
	CookingParameters::JudgeEvaluation GetCurrentEvaluation() const
	{
		return parameters_.Evaluate();
	}

	/// <summary>
	/// スライド12対応：5大パラメータのレーダーチャートデータ (0.0f ～ 1.0f) を取得
	/// </summary>
	CookingParameters::RadarChartData GetRadarChartData() const
	{
		return parameters_.GetRadarChartData();
	}

	// -----------------------------------------------------
	// VFX（エフェクト） / SE（効果音）向け出力インターフェース
	// -----------------------------------------------------

	/// <summary>
	/// 湯気（白煙・Steam）の発生強度 (0.0f ～ 1.0f)
	/// 温度100℃以上で水分蒸発が起きているときに発生（加熱中は強く、余熱中は穏やか）
	/// </summary>
	float GetSteamIntensity() const;

	/// <summary>
	/// 焦げ煙（黒煙・Smoke）の発生強度 (0.0f ～ 1.0f)
	/// 焦げ（burnt）が始まったときや、焦げつき警報時に立ち上る
	/// </summary>
	float GetSmokeIntensity() const;

	/// <summary>
	/// 炒め音（ジュワーッ / Sizzle）の再生フラグ
	/// 加熱中で、かつ適度な温度・水分があるときに true
	/// </summary>
	bool IsSizzling() const;

	/// <summary>
	/// 焦げ付き音（パチパチ・チリチリ）の警告音再生フラグ
	/// 鍋を振らずに焦げ猶予時間を超えて火に当て続けているときに true
	/// </summary>
	bool IsBurningAlert() const;

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

	const CookingTuningConfig& GetConfig() const { return config_; }
	CookingTuningConfig& GetConfig() { return config_; }

	/// <summary>
	/// 鍋が現在火元の上で加熱されているか
	/// </summary>
	bool IsHeating() const { return isHeating_; }

	// -----------------------------------------------------
	// デバッグ制御
	// -----------------------------------------------------
	void SetDebugPause(bool pause) { isDebugPause_ = pause; }
	bool IsDebugPause() const { return isDebugPause_; }

	void SetDebugForceCookingStation(bool force) { isDebugForceCookingStation_ = force; }
	bool IsDebugForceCookingStation() const { return isDebugForceCookingStation_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 火元による温度変化・自然冷却の更新
	/// </summary>
	void UpdateHeat(float deltaTime);

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
	CookingTuningConfig config_;

	// 調理場（コンロ近く）に立っているか（自動オフセット・加熱状態制御）
	bool isAtCookingStation_ = true;
	bool isWokShaking_ = false;
	float ladleStirStrength_ = 0.0f;

	bool isHeating_ = false;           // 現在加熱中かどうか
	float heatExposureTimer_ = 0.0f;   // 鍋を振らずに火にかけ続けている時間（焦げ判定用）

	// デバッグ用フラグ
	bool isDebugPause_ = false;                 // シミュレーション一時停止（パラメータ固定・手動観察用）
	bool isDebugForceCookingStation_ = false;   // 強制的に調理場にいる扱いにする（遠隔テスト用）

	// 常温（室温）
	const float kRoomTemperature = 25.0f;
	// 自然放熱係数
	const float kCoolingRate = 8.0f;
};
