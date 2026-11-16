#include "CookingSystem.h"

#include <imgui.h>
#include <cmath>
#include <algorithm>
#include "LGF/System.h"

namespace {
	constexpr float kIngredientCoolingFactor = 0.4f;      // 具材投入時の温度低下係数
	constexpr float kBurntDegradationThreshold = 30.0f;    // 焦げによる劣化開始閾値 (%)
	constexpr float kBurntAromaPenaltyRate = 1.0f;        // 焦げによる香り低下速度
	constexpr float kBurntFluffinessPenaltyRate = 0.8f;   // 焦げによるパラパラ度低下速度
}

CookingSystem::CookingSystem()
{
	// 外部パラメータファイルをロード。存在しなければデフォルト設定で自動生成
	if (!config_.LoadFromFile()) {
		config_.SaveToFile();
	}
	Reset();
}

void CookingSystem::Reset()
{
	parameters_ = CookingParameters{};
	parameters_.fluffiness = config_.initFluffiness;
	parameters_.aroma = config_.initAroma;
	parameters_.volume = config_.initVolume;
	parameters_.temperature = config_.initTemperature;
	parameters_.umami = config_.initUmami;
	parameters_.moisture = config_.initMoisture;
	parameters_.burnt = 0.0f;
	parameters_.shakeCount = 0;
	parameters_.verticalShakeCount = 0;
	parameters_.horizontalShakeCount = 0;
	parameters_.stirCount = 0;
	parameters_.addedIngredients.clear();
	parameters_.addedIngredients.push_back(IngredientType::Rice);

	isCookingMode_ = true;
	isWokShaking_ = false;
	ladleStirStrength_ = 0.0f;

	heatExposureTimer_ = 0.0f;
	isHeating_ = false;

	isDebugPause_ = false;
	isDebugForceCookingStation_ = false;
}

void CookingSystem::Update(
	float deltaTime,
	bool isCookingMode,
	bool isWokShaking,
	float ladleStirStrength)
{
	if (deltaTime <= 0.0f) {
		return;
	}

	// デバッグ用強制フラグが有効な場合は強制的に調理場（コンロ前）にいる扱いにする
	isCookingMode_ = isDebugForceCookingStation_ ? true : isCookingMode;
	isWokShaking_ = isWokShaking;
	ladleStirStrength_ = ladleStirStrength;

	// デバッグ用一時停止が有効な場合はシミュレーションの進行を止める
	if (isDebugPause_) {
		return;
	}

	// 1. 火元による熱伝達と冷却
	UpdateHeat(deltaTime);

	// 2. 水分蒸発と焦げの進行
	UpdateMoistureAndBurnt(deltaTime, isWokShaking);

	// 3. 香りの自然拡散
	UpdateAroma(deltaTime);

	// 4. おたまによる継続的な炒め処理
	if (ladleStirStrength > 0.05f) {
		float stirAmount = ladleStirStrength * deltaTime;
		// 炒めることで具材が均一になり旨味UP
		parameters_.umami += config_.ladleUmamiRate * stirAmount;
		// 水分が適正ならパラパラ度も微増
		if (parameters_.moisture < 50.0f && parameters_.temperature >= 100.0f) {
			parameters_.fluffiness += config_.ladleFluffinessRate * stirAmount;
		}
	}

	// パラメータを規定範囲内にクランプ
	parameters_.ClampAll();
}

void CookingSystem::ProcessLadleSwing(float swingDeltaZ, float deltaTime)
{
	if (deltaTime <= 0.0f) return;

	// スイングの速さ (速度) を算出
	float speed = std::abs(swingDeltaZ) / deltaTime;
	// 速度に応じた強度 (0.0f ～ 1.0f)
	float strength = std::clamp(speed / config_.ladleSwingSpeedScale, 0.0f, 1.0f);

	ladleStirStrength_ = strength;
	if (strength > config_.ladleSwingThreshold) {
		OnLadleStir(strength);
	}
}

bool CookingSystem::IsAnimalAttracted(const LGF::Vector3& animalPos) const
{
	float radius = GetAttractRadius();
	if (radius <= 0.1f) return false;

	// 中華レンジ（炒飯）の位置からの水平距離 (XZ平面)
	const auto& stovePos = heatSource_.GetPosition();
	float dx = animalPos.x - stovePos.x;
	float dz = animalPos.z - stovePos.z;
	float distSq = dx * dx + dz * dz;

	return distSq <= (radius * radius);
}

void CookingSystem::OnWokShake(float shakeIntensity)
{
	// デフォルトは縦あおり振りとして処理
	OnWokShake(WokShakeDirection::Vertical, shakeIntensity);
}

void CookingSystem::OnWokShake(WokShakeDirection direction, ShakeStrengthLevel strengthLevel)
{
	float intensity = 0.0f;
	switch (strengthLevel)
	{
	case ShakeStrengthLevel::Weak:
		intensity = config_.strengthWeakCoeff;
		break;
	case ShakeStrengthLevel::Medium:
		intensity = config_.strengthMediumCoeff;
		break;
	case ShakeStrengthLevel::Strong:
		intensity = config_.strengthStrongCoeff;
		break;
	default:
		return;
	}

	OnWokShake(direction, intensity);
}

void CookingSystem::OnWokShake(WokShakeDirection direction, float intensity)
{
	if (intensity <= 0.0f) return;

	// 鍋振り回数をカウント
	parameters_.shakeCount++;

	// 水分と温度による「黄金炒飯ゾーン」の効率倍率
	float efficiency = 1.0f;
	if (parameters_.temperature >= config_.goldenZoneTempMin) {
		if (parameters_.moisture >= config_.goldenZoneMoistMin && parameters_.moisture <= config_.goldenZoneMoistMax) {
			efficiency = config_.goldenZoneEfficiency;
		} else if (parameters_.moisture > config_.goldenZoneMoistMax) {
			efficiency = config_.wetZoneEfficiency;
		} else {
			efficiency = config_.dryZoneEfficiency;
		}
	}

	// 向きに応じた効果の適用
	switch (direction)
	{
	case WokShakeDirection::Vertical:
		// 縦振り（あおり・浮かし）：宙に舞うことでパラパラ度・香り・水分蒸発・冷却
		parameters_.verticalShakeCount++;
		parameters_.fluffiness += config_.shakeFluffinessVertical * intensity * efficiency;
		parameters_.aroma += config_.shakeAromaVertical * intensity;
		parameters_.moisture = std::max(0.0f, parameters_.moisture - config_.shakeMoistureLossVertical * intensity);
		parameters_.temperature = std::max(config_.roomTemperature, parameters_.temperature - config_.shakeCoolingVertical * intensity);
		break;

	case WokShakeDirection::Horizontal:
		// 横振り（水平回し・鍋肌）：均一加熱で旨味UP、鍋肌で焦げ防止、マイルドな冷却
		parameters_.horizontalShakeCount++;
		parameters_.umami += config_.shakeUmamiHorizontal * intensity;
		parameters_.fluffiness += config_.shakeFluffinessHorizontal * intensity * efficiency;
		parameters_.temperature = std::max(config_.roomTemperature, parameters_.temperature - config_.shakeCoolingHorizontal * intensity);
		break;

	default:
		break;
	}

	// 鍋を振ることで火元からの放置タイマーがリセットされる（焦げ付き防止）
	heatExposureTimer_ = 0.0f;

	// パラメータをクランプ
	parameters_.ClampAll();
}

void CookingSystem::OnLadleStir(float stirIntensity)
{
	if (stirIntensity <= 0.0f) return;

	parameters_.stirCount++;

	// 単発炒めアクションによる効果
	parameters_.umami += config_.ladleStirUmamiInstant * stirIntensity;

	// 水分量が適正で加熱中ならパラパラ度も微増
	if (parameters_.moisture < 50.0f && parameters_.temperature >= 100.0f) {
		parameters_.fluffiness += config_.ladleStirFluffinessInstant * stirIntensity;
	}

	// 具材をかき混ぜることで焦げタイマーを少し緩和
	heatExposureTimer_ = std::max(0.0f, heatExposureTimer_ - 0.5f * stirIntensity);

	parameters_.ClampAll();
}

void CookingSystem::AddIngredient(IngredientType type)
{
	Ingredient item{};
	item.type = type;

	auto applyBonus = [&](const std::string& name, const CookingTuningConfig::IngredientBonus& b) {
		item.name = name;
		item.volumeBonus = b.volume;
		item.umamiBonus = b.umami;
		item.aromaBonus = b.aroma;
		item.moistureBonus = b.moisture;
	};

	switch (type)
	{
	case IngredientType::Rice:
		applyBonus("追加ご飯", config_.ingRice);
		break;
	case IngredientType::Egg:
		applyBonus("溶き卵", config_.ingEgg);
		break;
	case IngredientType::Pork:
		applyBonus("豚肉（狩猟肉）", config_.ingPork);
		break;
	case IngredientType::Chicken:
		applyBonus("鶏肉（狩猟肉）", config_.ingChicken);
		break;
	case IngredientType::Beef:
		applyBonus("牛肉（大型狩猟肉）", config_.ingBeef);
		break;
	case IngredientType::GreenOnion:
		applyBonus("刻みネギ", config_.ingGreenOnion);
		break;
	case IngredientType::Seasoning:
		applyBonus("秘伝の調味料", config_.ingSeasoning);
		break;
	}

	AddIngredient(item);
}

void CookingSystem::AddIngredient(const Ingredient& ingredient)
{
	parameters_.addedIngredients.push_back(ingredient.type);
	parameters_.volume += ingredient.volumeBonus;
	parameters_.umami += ingredient.umamiBonus;
	parameters_.aroma += ingredient.aromaBonus;
	parameters_.moisture += ingredient.moistureBonus;

	// 冷たい具材が入ることで鍋の温度が少し低下する
	float coolingFromIngredient = ingredient.volumeBonus * kIngredientCoolingFactor;
	parameters_.temperature = std::max(config_.roomTemperature, parameters_.temperature - coolingFromIngredient);

	parameters_.ClampAll();
}

void CookingSystem::UpdateHeat(float deltaTime)
{
	// 調理モード（中華レンジ前にいる）かつコンロ点火中で加熱
	isHeating_ = heatSource_.IsActive() && isCookingMode_;

	if (isHeating_) {
		float targetTemp = config_.targetMaxTemperature;
		float heatOutput = config_.heatOutput;

		// 目標温度へ向けて上昇
		if (parameters_.temperature < targetTemp) {
			float rate = (targetTemp - parameters_.temperature) / targetTemp;
			parameters_.temperature += heatOutput * (0.3f + 0.7f * rate) * deltaTime;
		}

		// 鍋を振らずに火に当て続けている時間を計測
		heatExposureTimer_ += deltaTime;
	} else {
		// 火元から離れている時は自然冷却
		heatExposureTimer_ = 0.0f;
		if (parameters_.temperature > config_.roomTemperature) {
			float tempDiff = parameters_.temperature - config_.roomTemperature;
			parameters_.temperature -= (config_.naturalCoolingRate + tempDiff * config_.tempDiffCoolingCoeff) * deltaTime;
			if (parameters_.temperature < config_.roomTemperature) {
				parameters_.temperature = config_.roomTemperature;
			}
		}
	}
}

void CookingSystem::UpdateMoistureAndBurnt(float deltaTime, bool isWokShaking)
{
	if (isWokShaking) {
		heatExposureTimer_ = 0.0f;
	}

	// 水分蒸発処理：
	// 100℃以上で蒸発。温度上昇による蒸発量の急増を抑えるため sqrt で緩やかにスケーリング。
	// 加熱中はしっかり蒸発し、余熱時（!isHeating_）は微量（加熱中の約15%）だけ飛ぶ
	if (parameters_.temperature >= 100.0f && parameters_.moisture > 0.0f) {
		float tempFactor = std::sqrt(parameters_.temperature - 90.0f);
		float coeff = isHeating_ ? config_.evapRateCoeff : (config_.evapRateCoeff * config_.residualEvapMultiplier);
		float evapRate = tempFactor * coeff;

		parameters_.moisture -= evapRate * deltaTime;
		if (parameters_.moisture < 0.0f) {
			parameters_.moisture = 0.0f;
		}
	}

	// 焦げの進行判定：
	// 高温で水分が少なく、かつ一定時間振らずに火元で放置された場合
	if (isHeating_ && parameters_.temperature >= config_.burnStartTemp && parameters_.moisture <= config_.burnMaxMoisture) {
		if (heatExposureTimer_ > config_.burnGraceTime) {
			float burnRate = (heatExposureTimer_ - config_.burnGraceTime) * config_.burnRateCoeff;
			parameters_.burnt += burnRate * deltaTime;

			// 焦げると香りが悪くなり、パラパラ度も劣化
			if (parameters_.burnt > kBurntDegradationThreshold) {
				parameters_.aroma = std::max(0.0f, parameters_.aroma - kBurntAromaPenaltyRate * deltaTime);
				parameters_.fluffiness = std::max(0.0f, parameters_.fluffiness - kBurntFluffinessPenaltyRate * deltaTime);
			}
		}
	}
}

void CookingSystem::UpdateAroma(float deltaTime)
{
	// 香りは時間経過とともに大気中へゆっくり拡散・減衰（ただし動物を引き寄せるチャンス）
	if (parameters_.aroma > 0.0f) {
		parameters_.aroma -= config_.aromaDecayRate * deltaTime;
		if (parameters_.aroma < 0.0f) {
			parameters_.aroma = 0.0f;
		}
	}
}

float CookingSystem::GetSteamIntensity() const
{
	// 100℃未満、または水分が完全にゼロの場合は湯気が出ない
	if (parameters_.temperature < 100.0f || parameters_.moisture <= 0.0f) {
		return 0.0f;
	}

	// 温度に応じたベース強度 (100℃～200℃で 0.2～1.0)
	float tempFactor = std::clamp((parameters_.temperature - 100.0f) / 100.0f, 0.0f, 1.0f);
	float base = 0.2f + 0.8f * tempFactor;

	// 水分量によるスケール (水分が多いと濃く、乾いてくると薄くなる)
	float moistureFactor = std::clamp(parameters_.moisture / 25.0f, 0.1f, 1.0f);

	// 余熱時（火元から離れている時）は微量の湯気
	float stateMultiplier = isHeating_ ? 1.0f : config_.residualEvapMultiplier;

	return std::clamp(base * moistureFactor * stateMultiplier, 0.0f, 1.0f);
}

float CookingSystem::GetSmokeIntensity() const
{
	// 焦げ（burnt）が発生している場合は黒煙
	float burntSmoke = std::clamp(parameters_.burnt / 40.0f, 0.0f, 1.0f);

	// 焦げ猶予を超えて放置されているときの立ち上る煙
	float alertSmoke = 0.0f;
	if (isHeating_ && heatExposureTimer_ > config_.burnGraceTime && parameters_.temperature >= config_.burnStartTemp) {
		alertSmoke = std::clamp((heatExposureTimer_ - config_.burnGraceTime) / 3.0f, 0.0f, 1.0f);
	}

	return std::max(burntSmoke, alertSmoke);
}

bool CookingSystem::IsSizzling() const
{
	// 加熱中で、鍋の温度が90℃以上、かつ水分が残っていれば炒め音が鳴る
	return isHeating_ && (parameters_.temperature >= 90.0f) && (parameters_.moisture >= 3.0f) && (parameters_.burnt < 75.0f);
}

bool CookingSystem::IsBurningAlert() const
{
	// 焦げ始め直前〜焦げ進行中の警告音
	return isHeating_ && (heatExposureTimer_ > config_.burnGraceTime * 0.8f) && (parameters_.temperature >= config_.burnStartTemp);
}

// =========================================================
// ImGui デバッグ描画
// =========================================================

void CookingSystem::DrawImGuiDebug()
{
	if (ImGui::Begin("Cooking System Debug"))
	{
		DrawImGuiStatus();
		ImGui::Separator();
		DrawImGuiStove();
		ImGui::Separator();
		DrawImGuiTuningConfig();
		ImGui::Separator();
		DrawImGuiActions();
		ImGui::Separator();
		DrawImGuiJudgePreview();
	}
	ImGui::End();
}

void CookingSystem::DrawImGuiStatus()
{
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "=== 5 Major Parameters ===");
	ImGui::ProgressBar(parameters_.fluffiness / 100.0f, ImVec2(-1, 0), "Fluffiness");
	ImGui::ProgressBar(parameters_.aroma / 100.0f, ImVec2(-1, 0), "Aroma");
	ImGui::ProgressBar(parameters_.volume / 100.0f, ImVec2(-1, 0), "Volume");
	ImGui::ProgressBar(parameters_.umami / 100.0f, ImVec2(-1, 0), "Umami");
	ImGui::ProgressBar(parameters_.temperature / 300.0f, ImVec2(-1, 0), "Temperature");

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "=== Simulation Sub-Parameters ===");
	ImGui::ProgressBar(parameters_.moisture / 100.0f, ImVec2(-1, 0), "Moisture");
	ImGui::ProgressBar(parameters_.burnt / 100.0f, ImVec2(-1, 0), "Burnt (Char)");

	ImGui::Text("Heat Exposure Timer: %.2f sec (Grace: %.1f sec)", heatExposureTimer_, config_.burnGraceTime);
	ImGui::Text("Attract Radius: %.1f m (Max: %.1f m)", GetAttractRadius(), config_.attractMaxRadius);

	// VFX/SE状態
	ImGui::Text("VFX Steam: %.2f | Smoke: %.2f", GetSteamIntensity(), GetSmokeIntensity());
	ImGui::Text("SE Sizzle: [%s] | Burning Alert: [%s]",
		IsSizzling() ? "PLAYING" : "off",
		IsBurningAlert() ? "ALERT!" : "off");
}

void CookingSystem::DrawImGuiStove()
{
	ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "=== Stove (Heat Source) ===");
	ImGui::Text("Heating State: [%s]", isHeating_ ? "HEATING (ON)" : "Off / Away");

	ImGui::Checkbox("Cooking Mode (In Front of Stove)", &isCookingMode_);
	ImGui::SameLine();
	bool stoveActive = heatSource_.IsActive();
	if (ImGui::Checkbox("Stove Fire (Active)", &stoveActive)) {
		heatSource_.SetActive(stoveActive);
	}

	auto pos = heatSource_.GetPosition();
	if (ImGui::DragFloat3("Stove Position", &pos.x, 0.05f)) {
		heatSource_.SetPosition(pos);
	}
}

void CookingSystem::DrawImGuiTuningConfig()
{
	if (ImGui::CollapsingHeader("Cooking Tuning Config (External JSON & Balance)"))
	{
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "[File Management]");
		if (ImGui::Button("Save to JSON")) {
			config_.SaveToFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reload from JSON")) {
			config_.LoadFromFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset to Defaults")) {
			config_.ResetToDefault();
		}

		// 1. 熱・温度
		if (ImGui::TreeNode("1. Heat & Temperature")) {
			ImGui::DragFloat("Heat Output (degC/s)", &config_.heatOutput, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat("Target Max Temp (degC)", &config_.targetMaxTemperature, 1.0f, 100.0f, 400.0f, "%.1f");
			ImGui::DragFloat("Natural Cooling Rate", &config_.naturalCoolingRate, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Room Temp (degC)", &config_.roomTemperature, 0.5f, 0.0f, 50.0f, "%.1f");
			ImGui::DragFloat("Temp Diff Cooling Coeff", &config_.tempDiffCoolingCoeff, 0.001f, 0.0f, 0.1f, "%.4f");
			ImGui::TreePop();
		}

		// 2. 水分・焦げ
		if (ImGui::TreeNode("2. Moisture & Burnt")) {
			ImGui::DragFloat("Evap Rate Coeff", &config_.evapRateCoeff, 0.01f, 0.0f, 2.0f, "%.3f");
			ImGui::DragFloat("Residual Evap Mult", &config_.residualEvapMultiplier, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Burn Start Temp (degC)", &config_.burnStartTemp, 1.0f, 100.0f, 300.0f, "%.1f");
			ImGui::DragFloat("Burn Max Moisture (%)", &config_.burnMaxMoisture, 0.5f, 0.0f, 50.0f, "%.1f");
			ImGui::DragFloat("Burn Grace Time (s)", &config_.burnGraceTime, 0.2f, 0.0f, 30.0f, "%.1f");
			ImGui::DragFloat("Burn Rate Coeff", &config_.burnRateCoeff, 0.1f, 0.0f, 10.0f, "%.2f");
			ImGui::TreePop();
		}

		// 3. 鍋振り
		if (ImGui::TreeNode("3. Wok Shake Dynamics")) {
			ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "[Vertical Shake (Aori)]");
			ImGui::DragFloat("Fluffiness Up (Vert)", &config_.shakeFluffinessVertical, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Aroma Up (Vert)", &config_.shakeAromaVertical, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Moisture Loss (Vert)", &config_.shakeMoistureLossVertical, 0.05f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Cooling (Vert degC)", &config_.shakeCoolingVertical, 0.5f, 0.0f, 50.0f, "%.1f");

			ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "[Horizontal Shake (Mawashi)]");
			ImGui::DragFloat("Umami Up (Hori)", &config_.shakeUmamiHorizontal, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Fluffiness Up (Hori)", &config_.shakeFluffinessHorizontal, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Cooling (Hori degC)", &config_.shakeCoolingHorizontal, 0.5f, 0.0f, 50.0f, "%.1f");

			ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "[Golden Zone]");
			ImGui::DragFloat("Golden Temp Min", &config_.goldenZoneTempMin, 1.0f, 50.0f, 250.0f, "%.1f");
			ImGui::DragFloat("Golden Moist Min", &config_.goldenZoneMoistMin, 0.5f, 0.0f, 50.0f, "%.1f");
			ImGui::DragFloat("Golden Moist Max", &config_.goldenZoneMoistMax, 0.5f, 10.0f, 80.0f, "%.1f");
			ImGui::DragFloat("Golden Efficiency", &config_.goldenZoneEfficiency, 0.05f, 0.5f, 3.0f, "%.2f");
			ImGui::DragFloat("Wet Efficiency", &config_.wetZoneEfficiency, 0.05f, 0.1f, 1.5f, "%.2f");
			ImGui::DragFloat("Dry Efficiency", &config_.dryZoneEfficiency, 0.05f, 0.05f, 1.5f, "%.2f");

			ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "[Strength Scale]");
			ImGui::DragFloat("Weak Coeff", &config_.strengthWeakCoeff, 0.05f, 0.1f, 2.0f, "%.2f");
			ImGui::DragFloat("Medium Coeff", &config_.strengthMediumCoeff, 0.05f, 0.1f, 3.0f, "%.2f");
			ImGui::DragFloat("Strong Coeff", &config_.strengthStrongCoeff, 0.05f, 0.1f, 5.0f, "%.2f");
			ImGui::TreePop();
		}

		// 4. おたま
		if (ImGui::TreeNode("4. Ladle Stir Dynamics")) {
			ImGui::DragFloat("Ladle Umami Rate (/s)", &config_.ladleUmamiRate, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Ladle Fluffiness Rate (/s)", &config_.ladleFluffinessRate, 0.1f, 0.0f, 20.0f, "%.2f");
			ImGui::DragFloat("Instant Umami (Hit)", &config_.ladleStirUmamiInstant, 0.1f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Instant Fluffiness (Hit)", &config_.ladleStirFluffinessInstant, 0.1f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Swing Speed Scale", &config_.ladleSwingSpeedScale, 0.1f, 0.1f, 10.0f, "%.2f");
			ImGui::DragFloat("Swing Threshold", &config_.ladleSwingThreshold, 0.02f, 0.05f, 1.0f, "%.2f");
			ImGui::TreePop();
		}

		// 5. 香り
		if (ImGui::TreeNode("5. Aroma & Attraction")) {
			ImGui::DragFloat("Aroma Decay Rate (/s)", &config_.aromaDecayRate, 0.05f, 0.0f, 5.0f, "%.2f");
			ImGui::DragFloat("Max Attract Radius (m)", &config_.attractMaxRadius, 1.0f, 5.0f, 100.0f, "%.1f");
			ImGui::TreePop();
		}

		// 6. 初期値
		if (ImGui::TreeNode("6. Initial Dish Values")) {
			ImGui::DragFloat("Init Fluffiness", &config_.initFluffiness, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat("Init Aroma", &config_.initAroma, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat("Init Volume", &config_.initVolume, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat("Init Temp (degC)", &config_.initTemperature, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat("Init Umami", &config_.initUmami, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat("Init Moisture", &config_.initMoisture, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::TreePop();
		}

		// 7. 食材ボーナス
		if (ImGui::TreeNode("7. Ingredient Bonuses")) {
			auto drawBonusUI = [](const char* name, CookingTuningConfig::IngredientBonus& b) {
				if (ImGui::TreeNode(name)) {
					ImGui::DragFloat("Volume Bonus", &b.volume, 0.5f, 0.0f, 50.0f, "%.1f");
					ImGui::DragFloat("Umami Bonus", &b.umami, 0.5f, 0.0f, 50.0f, "%.1f");
					ImGui::DragFloat("Aroma Bonus", &b.aroma, 0.5f, 0.0f, 50.0f, "%.1f");
					ImGui::DragFloat("Moisture Bonus", &b.moisture, 0.5f, -20.0f, 50.0f, "%.1f");
					ImGui::TreePop();
				}
			};
			drawBonusUI("Rice (Base)", config_.ingRice);
			drawBonusUI("Egg", config_.ingEgg);
			drawBonusUI("Pork (Hunt)", config_.ingPork);
			drawBonusUI("Chicken (Hunt)", config_.ingChicken);
			drawBonusUI("Beef (Large Hunt)", config_.ingBeef);
			drawBonusUI("Green Onion", config_.ingGreenOnion);
			drawBonusUI("Seasoning", config_.ingSeasoning);
			ImGui::TreePop();
		}

		// 8. 審査員配点・閾値
		if (ImGui::TreeNode("8. Judge Scoring")) {
			ImGui::DragFloat("Fluffiness Weight", &config_.judge.fluffinessWeight, 0.02f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Umami Weight", &config_.judge.umamiWeight, 0.02f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Volume Weight", &config_.judge.volumeWeight, 0.02f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Burn Penalty Threshold (%)", &config_.judge.burnPenaltyThreshold, 1.0f, 0.0f, 50.0f, "%.1f %%");
			ImGui::DragFloat("Burn Penalty Rate", &config_.judge.burnPenaltyRate, 0.1f, 0.0f, 5.0f, "%.2f");
			ImGui::DragFloat("Burnt Instant Fail (%)", &config_.judge.burntFailThreshold, 1.0f, 20.0f, 100.0f, "%.1f %%");
			ImGui::DragInt("S Rank Score", &config_.judge.scoreMasterpiece, 1, 50, 100);
			ImGui::DragInt("A Rank Score", &config_.judge.scoreDelicious, 1, 40, 95);
			ImGui::DragInt("B Rank Score", &config_.judge.scoreAcceptable, 1, 20, 80);
			ImGui::TreePop();
		}
	}
}

void CookingSystem::DrawImGuiActions()
{
	ImGui::TextColored(ImVec4(0.8f, 0.5f, 1.0f, 1.0f), "=== Manual Action Triggers ===");

	// 鍋振りテスト
	static int selectedDir = 1;
	static int selectedLevel = 1;
	ImGui::RadioButton("Vertical (Aori)", &selectedDir, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Horizontal (Mawashi)", &selectedDir, 2);

	ImGui::RadioButton("Weak", &selectedLevel, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Medium", &selectedLevel, 2);
	ImGui::SameLine();
	ImGui::RadioButton("Strong", &selectedLevel, 3);

	if (ImGui::Button("Trigger Wok Shake")) {
		WokShakeDirection dir = (selectedDir == 1) ? WokShakeDirection::Vertical : WokShakeDirection::Horizontal;
		ShakeStrengthLevel lvl = ShakeStrengthLevel::Medium;
		if (selectedLevel == 1) lvl = ShakeStrengthLevel::Weak;
		else if (selectedLevel == 3) lvl = ShakeStrengthLevel::Strong;
		OnWokShake(dir, lvl);
	}

	ImGui::SameLine();
	if (ImGui::Button("Trigger Ladle Stir")) {
		OnLadleStir(1.0f);
	}

	// 具材追加テスト
	if (ImGui::TreeNode("Add Ingredients Test")) {
		if (ImGui::Button("Rice")) AddIngredient(IngredientType::Rice);
		ImGui::SameLine();
		if (ImGui::Button("Egg")) AddIngredient(IngredientType::Egg);
		ImGui::SameLine();
		if (ImGui::Button("Pork")) AddIngredient(IngredientType::Pork);
		ImGui::SameLine();
		if (ImGui::Button("Chicken")) AddIngredient(IngredientType::Chicken);
		ImGui::SameLine();
		if (ImGui::Button("Beef")) AddIngredient(IngredientType::Beef);
		ImGui::SameLine();
		if (ImGui::Button("Green Onion")) AddIngredient(IngredientType::GreenOnion);
		ImGui::SameLine();
		if (ImGui::Button("Seasoning")) AddIngredient(IngredientType::Seasoning);
		ImGui::TreePop();
	}

	// リセット & 一時停止
	if (ImGui::Button("Reset Dish Parameters")) {
		Reset();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Pause Simulation", &isDebugPause_);
}

void CookingSystem::DrawImGuiJudgePreview()
{
	ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.8f, 1.0f), "=== Judge Man Preview ===");
	auto eval = GetCurrentEvaluation();
	const char* gradeStr = "Unknown";
	ImVec4 gradeColor(1, 1, 1, 1);
	switch (eval.grade)
	{
	case CookingParameters::JudgeGrade::Masterpiece:
		gradeStr = "S: Masterpiece (Ultimate Fried Rice)";
		gradeColor = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);
		break;
	case CookingParameters::JudgeGrade::Delicious:
		gradeStr = "A: Delicious (Splendid)";
		gradeColor = ImVec4(0.2f, 1.0f, 0.4f, 1.0f);
		break;
	case CookingParameters::JudgeGrade::Acceptable:
		gradeStr = "B: Acceptable (Passing)";
		gradeColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
		break;
	case CookingParameters::JudgeGrade::Unacceptable:
		gradeStr = "C: Unacceptable (Failed)";
		gradeColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
		break;
	}

	ImGui::TextColored(gradeColor, "Rank: %s", gradeStr);
	ImGui::Text("Total Score: %d / 100", eval.totalScore);
	ImGui::TextWrapped("Comment: \"%s\"", eval.commentEn.c_str());

	// 5軸レーダーチャートプレビュー
	auto radar = eval.radarChart;
	ImGui::Text("Radar: Fluff[%.2f] Aroma[%.2f] Vol[%.2f] Temp[%.2f] Umami[%.2f] (Avg: %.2f)",
		radar.fluffiness, radar.aroma, radar.volume, radar.temperature, radar.umami, radar.GetAverage());
}
