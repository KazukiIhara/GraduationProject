#pragma once

#include "CookingParameters.h"
#include "CookingConfig.h"
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
	/// 毎フレームの調理更新処理（ステートベース：調理モード・鍋振り・炒め入力で更新）
	/// 調理場にセットされているかで自動加熱/冷却を管理します
	/// </summary>
	void Update(
		float deltaTime,
		bool isCookingMode,
		bool isWokShaking,
		float ladleStirStrength = 0.0f
	);

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
	/// 米（ご飯）を中華鍋に投入
	/// </summary>
	/// <param name="amount">追加する米の量（g）</param>
	void AddRice(float amount = 100.0f);

	/// <summary>
	/// 食材を中華鍋に投入（CookingFriedRiceのIngredientTypeに対応）
	/// </summary>
	/// <param name="type">投入する食材の種類</param>
	/// <param name="amount">投入量（標準: 1.0f）</param>
	void AddIngredient(IngredientType type, float amount = 1.0f);

	/// <summary>
	/// 食材を中華鍋に投入（食材名直接指定：未知の新規食材にも対応）
	/// </summary>
	/// <param name="name">食材の名称（例: "Egg", "Meat", "Tomato" 等）</param>
	/// <param name="amount">投入量（標準: 1.0f）</param>
	void AddIngredient(const std::string& name, float amount = 1.0f);

	/// <summary>
	/// 食材投入（詳細定義指定・互換用）
	/// </summary>
	void AddIngredient(const Ingredient& ingredient);

	/// <summary>
	/// CookingFriedRice の状態（米の量や投入具材）からパラメータを一括同期
	/// </summary>
	void SyncWithFriedRice(const CookingFriedRice& friedRice);

	// -----------------------------------------------------
	// 外部（中華レンジ・鍋・動物・審査員）との接続支援
	// -----------------------------------------------------

	/// <summary>
	/// 調理モード中（中華レンジ前に立ち、鍋がセットされているか）を設定
	/// true: 加熱状態となり、放置すると焦げが進行します
	/// false: コンロから鍋が離れ、加熱が停止（自然放熱）します
	/// </summary>
	void SetCookingMode(bool isCooking) { isCookingMode_ = isCooking; }
	bool IsCookingMode() const { return isCookingMode_; }

	// 互換用エイリアス
	void SetAtCookingStation(bool isAtStation) { isCookingMode_ = isAtStation; }
	bool IsAtCookingStation() const { return isCookingMode_; }

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
	float GetAttractRadius() const
	{
		return config_.attractMaxRadius * GetAttractAnimalStrength();
	}

	/// <summary>
	/// 指定座標の動物が炒飯の匂い感知範囲に入っているか判定
	/// </summary>
	bool IsAnimalAttracted(const LGF::Vector3& animalPos) const;

	/// <summary>
	/// 評論家による現在の炒飯の評価を取得
	/// </summary>
	CookingParameters::JudgeEvaluation GetCurrentEvaluation() const
	{
		return parameters_.Evaluate(config_.judge);
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

	// -----------------------------------------------------
	// ImGui 分割描画ヘルパー
	// -----------------------------------------------------
	void DrawImGuiStatus();
	void DrawImGuiStove();
	void DrawImGuiTuningConfig();
	void DrawImGuiActions();
	void DrawImGuiJudgePreview();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	CookingParameters parameters_;
	HeatSource heatSource_;
	CookingTuningConfig config_;

	// 料理モード（中華レンジ前に立ち、鍋がセットされているか）
	bool isCookingMode_ = true;
	bool isWokShaking_ = false;
	float ladleStirStrength_ = 0.0f;

	bool isHeating_ = false;           // 現在加熱中かどうか
	float heatExposureTimer_ = 0.0f;   // 鍋を振らずに火にかけ続けている時間（焦げ判定用）

	// デバッグ用フラグ
	bool isDebugPause_ = false;                 // シミュレーション一時停止（パラメータ固定・手動観察用）
	bool isDebugForceCookingStation_ = false;   // 強制的に調理場にいる扱いにする（遠隔テスト用）
};
