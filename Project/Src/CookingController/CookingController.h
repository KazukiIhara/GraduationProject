#pragma once

#include <memory>
#include "Wok.h"
#include "Ladle.h"
#include "CookingSystem.h"

// =========================================================
// 調理器具管理クラス
// =========================================================
class CookingController
{
public:
	// =========================================================
	// Public Methods
	// =========================================================
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CookingController();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="isCookingMode">プレイヤーが調理場に立っているか（料理モード中か）</param>
	void Update(bool isCookingMode = true);

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
	/// 調理システムへの参照を取得
	/// </summary>
	CookingSystem& GetCookingSystem() { return *cookingSystem_; }
	const CookingSystem& GetCookingSystem() const { return *cookingSystem_; }

	/// <summary>
	/// 中華鍋への参照を取得
	/// </summary>
	Wok& GetWok() { return *wok_; }
	const Wok& GetWok() const { return *wok_; }

	/// <summary>
	/// おたまへの参照を取得
	/// </summary>
	Ladle& GetLadle() { return *ladle_; }
	const Ladle& GetLadle() const { return *ladle_; }

	// -----------------------------------------------------
	// スライド12対応：レーダーチャート & 評価データ
	// -----------------------------------------------------
	CookingParameters::RadarChartData GetRadarChartData() const { return cookingSystem_->GetRadarChartData(); }
	CookingParameters::JudgeEvaluation GetCurrentEvaluation() const { return cookingSystem_->GetCurrentEvaluation(); }

	// -----------------------------------------------------
	// VFX（エフェクト） / SE（効果音）向け出力
	// -----------------------------------------------------

	/// <summary>
	/// 湯気（白煙・Steam）の発生強度 (0.0f ～ 1.0f)
	/// </summary>
	float GetSteamIntensity() const { return cookingSystem_->GetSteamIntensity(); }

	/// <summary>
	/// 焦げ煙（黒煙・Smoke）の発生強度 (0.0f ～ 1.0f)
	/// </summary>
	float GetSmokeIntensity() const { return cookingSystem_->GetSmokeIntensity(); }

	/// <summary>
	/// 炒め音（ジュワーッ / Sizzle）の再生フラグ
	/// </summary>
	bool IsSizzling() const { return cookingSystem_->IsSizzling(); }

	/// <summary>
	/// 焦げ付き音（パチパチ警告）の再生フラグ
	/// </summary>
	bool IsBurningAlert() const { return cookingSystem_->IsBurningAlert(); }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// 調理器具クラス
	std::unique_ptr<Wok> wok_;
	std::unique_ptr<Ladle> ladle_;

	// 調理シミュレーションシステム
	std::unique_ptr<CookingSystem> cookingSystem_;

	// 直前の鍋振り状態（トリガー検知用）
	bool prevWokShaking_ = false;
	float prevLadleOffsetZ_ = 0.0f;
};

