#include "CookingSystem.h"

#include <imgui.h>
#include <cmath>
#include <algorithm>
#include "LGF/System.h"

CookingSystem::CookingSystem()
{
	Reset();
}

void CookingSystem::Reset()
{
	parameters_ = CookingParameters{};
	parameters_.fluffiness = 10.0f;
	parameters_.aroma = 0.0f;
	parameters_.volume = 30.0f;     // ご飯1人前
	parameters_.temperature = kRoomTemperature;
	parameters_.umami = 15.0f;
	parameters_.moisture = 55.0f;    // 炊き立てご飯の水分
	parameters_.burnt = 0.0f;
	parameters_.shakeCount = 0;
	parameters_.verticalShakeCount = 0;
	parameters_.horizontalShakeCount = 0;
	parameters_.stirCount = 0;
	parameters_.addedIngredients.clear();
	parameters_.addedIngredients.push_back(IngredientType::Rice);

	isAtCookingStation_ = true;
	isWokShaking_ = false;
	ladleStirStrength_ = 0.0f;

	heatExposureTimer_ = 0.0f;
	isHeating_ = false;

	isDebugPause_ = false;
	isDebugForceCookingStation_ = false;
}

void CookingSystem::Update()
{
	float dt = static_cast<float>(LGF::System::DeltaTime());
	Update(dt, isAtCookingStation_, isWokShaking_, ladleStirStrength_);
}

void CookingSystem::Update(
	float deltaTime,
	bool isAtCookingStation,
	bool isWokShaking,
	float ladleStirStrength)
{
	if (deltaTime <= 0.0f) {
		return;
	}

	// デバッグ用強制フラグが有効な場合は強制的に調理場（コンロ前）にいる扱いにする
	isAtCookingStation_ = isDebugForceCookingStation_ ? true : isAtCookingStation;
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
	float strength = std::clamp(speed / 1.5f, 0.0f, 1.0f);

	ladleStirStrength_ = strength;
	if (strength > 0.3f) {
		OnLadleStir(strength);
	}
}

bool CookingSystem::IsAnimalAttracted(const LGF::Vector3& animalPos, float maxRadius) const
{
	float radius = GetAttractRadius(maxRadius);
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
	case ShakeStrengthLevel::None:
	default:
		return;
	}

	OnWokShake(direction, intensity);
}

void CookingSystem::OnWokShake(WokShakeDirection direction, float intensity)
{
	if (direction == WokShakeDirection::None || intensity <= 0.0f) {
		return;
	}

	parameters_.shakeCount++;
	heatExposureTimer_ = 0.0f; // 鍋底の熱を分散させて焦げを防止

	// 水分状態に応じたパラパラ度上昇効率（温度が高く水分が適正なときに適正ゾーン）
	float efficiency = 0.5f;
	if (parameters_.temperature >= 120.0f) {
		if (parameters_.moisture >= 10.0f && parameters_.moisture <= 35.0f) {
			efficiency = 1.3f; // 適正ゾーン
		} else if (parameters_.moisture > 35.0f) {
			efficiency = 0.6f; // まだ水分多め
		} else {
			efficiency = 0.2f; // 水分が飛びすぎてパサパサ
		}
	}

	// 向きに応じた調理効果
	if (direction == WokShakeDirection::Vertical) {
		// -----------------------------------------------------
		// 縦振り（あおり・浮かし）：具材が宙に舞い、熱風が通る
		// -----------------------------------------------------
		parameters_.verticalShakeCount++;

		// パラパラ度大幅UP
		parameters_.fluffiness += config_.shakeFluffinessVertical * intensity * efficiency;

		// 湯気・香気が大気中に一気に広がる
		if (parameters_.temperature >= 100.0f) {
			parameters_.aroma += config_.shakeAromaVertical * intensity;
		}

		// 宙を舞うことで余分な水分が飛ぶ
		if (parameters_.temperature >= 100.0f) {
			parameters_.moisture = std::max(0.0f, parameters_.moisture - config_.shakeMoistureLossVertical * intensity);
		}
	}
	else if (direction == WokShakeDirection::Horizontal) {
		// -----------------------------------------------------
		// 横振り（水平回し・鍋肌）：具材が鍋肌を滑り均一に混ざり合う
		// -----------------------------------------------------
		parameters_.horizontalShakeCount++;

		// 全体が均一になり旨味UP
		parameters_.umami += config_.shakeUmamiHorizontal * intensity;

		// 鍋底の具材が入れ替わり、パラパラ度も適度にUP
		parameters_.fluffiness += config_.shakeFluffinessHorizontal * intensity * (efficiency * 0.7f);

		// 湯気・香気も適度に立ち上がる
		if (parameters_.temperature >= 100.0f) {
			parameters_.aroma += (config_.shakeAromaVertical * 0.4f) * intensity;
		}
	}

	parameters_.ClampAll();
}

void CookingSystem::OnLadleStir(float stirIntensity)
{
	parameters_.stirCount++;

	// おたまによる撹拌：具材の調和（旨味UP）
	parameters_.umami += 0.8f * stirIntensity;

	// パラパラ度微増
	if (parameters_.temperature >= 90.0f) {
		parameters_.fluffiness += 0.4f * stirIntensity;
	}

	parameters_.ClampAll();
}

void CookingSystem::AddIngredient(IngredientType type)
{
	Ingredient item{};
	item.type = type;

	switch (type)
	{
	case IngredientType::Rice:
		item.name = "追加ご飯";
		item.volumeBonus = 15.0f;
		item.umamiBonus = 2.0f;
		item.aromaBonus = 1.0f;
		item.moistureBonus = 15.0f;
		break;
	case IngredientType::Egg:
		item.name = "溶き卵";
		item.volumeBonus = 5.0f;
		item.umamiBonus = 6.0f;
		item.aromaBonus = 4.0f;
		item.moistureBonus = 8.0f;
		break;
	case IngredientType::Pork:
		item.name = "豚肉（狩猟肉）";
		item.volumeBonus = 10.0f;
		item.umamiBonus = 10.0f;
		item.aromaBonus = 6.0f;
		item.moistureBonus = 4.0f;
		break;
	case IngredientType::Chicken:
		item.name = "鶏肉（狩猟肉）";
		item.volumeBonus = 8.0f;
		item.umamiBonus = 8.0f;
		item.aromaBonus = 5.0f;
		item.moistureBonus = 4.0f;
		break;
	case IngredientType::Beef:
		item.name = "牛肉（大型狩猟肉）";
		item.volumeBonus = 15.0f;
		item.umamiBonus = 14.0f;
		item.aromaBonus = 8.0f;
		item.moistureBonus = 5.0f;
		break;
	case IngredientType::GreenOnion:
		item.name = "刻みネギ";
		item.volumeBonus = 3.0f;
		item.umamiBonus = 4.0f;
		item.aromaBonus = 8.0f;
		item.moistureBonus = 2.0f;
		break;
	case IngredientType::Seasoning:
		item.name = "秘伝の調味料";
		item.volumeBonus = 0.0f;
		item.umamiBonus = 10.0f;
		item.aromaBonus = 6.0f;
		item.moistureBonus = 1.0f;
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
	float coolingFromIngredient = ingredient.volumeBonus * 0.4f;
	parameters_.temperature = std::max(kRoomTemperature, parameters_.temperature - coolingFromIngredient);

	parameters_.ClampAll();
}

void CookingSystem::UpdateHeat(float deltaTime)
{
	// 調理場ステート（isAtCookingStation_）で加熱を制御
	isHeating_ = heatSource_.IsActive() && isAtCookingStation_;

	if (isHeating_) {
		float targetTemp = heatSource_.GetTargetMaxTemperature();
		float heatOutput = heatSource_.IsActive() ? config_.heatOutput : 0.0f;

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
		if (parameters_.temperature > kRoomTemperature) {
			float tempDiff = parameters_.temperature - kRoomTemperature;
			parameters_.temperature -= (config_.coolingRate + tempDiff * 0.015f) * deltaTime;
			if (parameters_.temperature < kRoomTemperature) {
				parameters_.temperature = kRoomTemperature;
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
			if (parameters_.burnt > 30.0f) {
				parameters_.aroma = std::max(0.0f, parameters_.aroma - 1.0f * deltaTime);
				parameters_.fluffiness = std::max(0.0f, parameters_.fluffiness - 0.8f * deltaTime);
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

void CookingSystem::DrawImGuiDebug()
{
#ifdef _DEBUG
	if (ImGui::Begin("Fried Rice Cooking System")) {
		// -----------------------------------------------------
		// 0. デバッグ操作（シミュレーション一時停止・強制調理場フラグ）
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.8f, 1.0f), "=== Debug Controls ===");
		ImGui::Checkbox("Pause Simulation", &isDebugPause_);
		ImGui::SameLine();
		ImGui::Checkbox("Force At Station", &isDebugForceCookingStation_);
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Force player to be treated as standing at cooking station (auto heat)");
		}

		ImGui::Separator();

		// -----------------------------------------------------
		// 1. スライド記載の5大パラメータ確認＆直接調整
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "=== 5 Big Parameters (Realtime Adjust) ===");

		// パラパラ度
		ImGui::SliderFloat("Fluffiness (Parapara)", &parameters_.fluffiness, 0.0f, 100.0f, "%.1f");

		// 香り（動物誘引）
		ImGui::SliderFloat("Aroma (Smell)", &parameters_.aroma, 0.0f, 100.0f, "%.1f");
		ImGui::SameLine();
		ImGui::TextDisabled("(Radius: %.1fm)", GetAttractRadius());

		// ボリューム
		ImGui::SliderFloat("Volume", &parameters_.volume, 0.0f, 100.0f, "%.1f");

		// 温度
		ImGui::SliderFloat("Temperature (C)", &parameters_.temperature, 0.0f, 300.0f, "%.1f C");

		// 旨味
		ImGui::SliderFloat("Umami", &parameters_.umami, 0.0f, 100.0f, "%.1f");

		ImGui::Separator();

		// -----------------------------------------------------
		// 2. 状態・物理補助パラメータ
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "=== State & Physics Adjust ===");
		ImGui::SliderFloat("Moisture (Water %)", &parameters_.moisture, 0.0f, 100.0f, "%.1f%%");
		ImGui::SliderFloat("Burnt (%)", &parameters_.burnt, 0.0f, 100.0f, "%.1f%%");

		ImGui::Text("Heat Timer: %.2f s | Shakes: %d (V:%d, H:%d) | Stirs: %d",
			heatExposureTimer_, parameters_.shakeCount, parameters_.verticalShakeCount, parameters_.horizontalShakeCount, parameters_.stirCount);

		ImGui::Separator();

		// -----------------------------------------------------
		// 3. 火元（中華レンジ）コントロール
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "=== Stove (Heat Source) ===");
		ImGui::Text("Heating State: [%s]", isHeating_ ? "HEATING (ON)" : "Off / Away");

		ImGui::Checkbox("At Cooking Station (Live)", &isAtCookingStation_);
		ImGui::SameLine();
		bool stoveActive = heatSource_.IsActive();
		if (ImGui::Checkbox("Stove Fire (Active)", &stoveActive)) {
			heatSource_.SetActive(stoveActive);
		}

		auto pos = heatSource_.GetPosition();
		if (ImGui::DragFloat3("Stove Position", &pos.x, 0.05f)) {
			heatSource_.SetPosition(pos);
		}

		ImGui::Separator();

		// -----------------------------------------------------
		// 4. ゲーム物理・シミュレーションのチューニングパラメータ (Config)
		// -----------------------------------------------------
		if (ImGui::CollapsingHeader("Cooking Tuning Config (Game Balance)")) {
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "[Heat & Cooling]");
			ImGui::DragFloat("Heat Output (C/s)", &config_.heatOutput, 0.5f, 5.0f, 80.0f, "%.1f C/s");
			ImGui::DragFloat("Natural Cooling Rate (C/s)", &config_.coolingRate, 0.1f, 0.5f, 15.0f, "%.1f C/s");

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "[Evaporation & Burning]");
			ImGui::DragFloat("Evap Rate Coeff", &config_.evapRateCoeff, 0.005f, 0.05f, 0.80f, "%.3f");
			ImGui::DragFloat("Residual Evap Multiplier", &config_.residualEvapMultiplier, 0.01f, 0.0f, 0.5f, "%.2fx");
			ImGui::DragFloat("Burn Grace Time (s)", &config_.burnGraceTime, 0.5f, 1.0f, 15.0f, "%.1f s");
			ImGui::DragFloat("Burn Speed Coeff", &config_.burnRateCoeff, 0.1f, 0.2f, 5.0f, "%.1f");
			ImGui::DragFloat("Burn Start Temp (C)", &config_.burnStartTemp, 2.0f, 120.0f, 250.0f, "%.0f C");
			ImGui::DragFloat("Burn Max Moisture (%)", &config_.burnMaxMoisture, 1.0f, 5.0f, 30.0f, "%.0f%%");

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "[Wok Shake Actions (Vertical / Horizontal)]");
			ImGui::DragFloat("Vertical Fluffiness Bonus", &config_.shakeFluffinessVertical, 0.2f, 1.0f, 20.0f, "%.1f");
			ImGui::DragFloat("Vertical Aroma Bonus", &config_.shakeAromaVertical, 0.5f, 1.0f, 20.0f, "%.1f");
			ImGui::DragFloat("Vertical Moisture Loss", &config_.shakeMoistureLossVertical, 0.1f, 0.0f, 5.0f, "%.1f");
			ImGui::DragFloat("Horizontal Umami Bonus", &config_.shakeUmamiHorizontal, 0.2f, 0.5f, 15.0f, "%.1f");
			ImGui::DragFloat("Horizontal Fluffiness Bonus", &config_.shakeFluffinessHorizontal, 0.2f, 0.5f, 10.0f, "%.1f");

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "[Strength Level Multipliers]");
			ImGui::DragFloat("Weak Multiplier", &config_.strengthWeakCoeff, 0.05f, 0.1f, 1.0f, "%.2f");
			ImGui::DragFloat("Medium Multiplier", &config_.strengthMediumCoeff, 0.05f, 0.5f, 2.0f, "%.2f");
			ImGui::DragFloat("Strong Multiplier", &config_.strengthStrongCoeff, 0.05f, 1.0f, 3.0f, "%.2f");

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "[Ladle Stir & Aroma]");
			ImGui::DragFloat("Ladle Umami Rate (/s)", &config_.ladleUmamiRate, 0.2f, 0.5f, 10.0f, "%.1f");
			ImGui::DragFloat("Ladle Fluffiness Rate (/s)", &config_.ladleFluffinessRate, 0.2f, 0.5f, 10.0f, "%.1f");
			ImGui::DragFloat("Aroma Decay Rate (/s)", &config_.aromaDecayRate, 0.05f, 0.05f, 2.0f, "%.2f");
		}

		ImGui::Separator();

		// -----------------------------------------------------
		// 5. アクション検証 & プリセット
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "=== Action Simulation (Direction & Strength) ===");
		ImGui::Text("Vertical Shake (Aori):");
		if (ImGui::Button("Vert [Weak]"))   OnWokShake(WokShakeDirection::Vertical, ShakeStrengthLevel::Weak);
		ImGui::SameLine();
		if (ImGui::Button("Vert [Med]"))    OnWokShake(WokShakeDirection::Vertical, ShakeStrengthLevel::Medium);
		ImGui::SameLine();
		if (ImGui::Button("Vert [Strong]")) OnWokShake(WokShakeDirection::Vertical, ShakeStrengthLevel::Strong);

		ImGui::Text("Horizontal Shake (Mawashi):");
		if (ImGui::Button("Horiz [Weak]"))   OnWokShake(WokShakeDirection::Horizontal, ShakeStrengthLevel::Weak);
		ImGui::SameLine();
		if (ImGui::Button("Horiz [Med]"))    OnWokShake(WokShakeDirection::Horizontal, ShakeStrengthLevel::Medium);
		ImGui::SameLine();
		if (ImGui::Button("Horiz [Strong]")) OnWokShake(WokShakeDirection::Horizontal, ShakeStrengthLevel::Strong);

		ImGui::Text("Ladle Action:");
		if (ImGui::Button("Ladle Stir (1x)")) {
			OnLadleStir(1.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset All")) {
			Reset();
		}

		// プリセット適用ボタン
		ImGui::Text("Presets:");
		if (ImGui::Button("Golden Ready (A)")) {
			parameters_.fluffiness = 70.0f;
			parameters_.umami = 75.0f;
			parameters_.volume = 60.0f;
			parameters_.temperature = 180.0f;
			parameters_.moisture = 20.0f;
			parameters_.burnt = 0.0f;
		}
		ImGui::SameLine();
		if (ImGui::Button("Ultimate Fried Rice (S)")) {
			parameters_.fluffiness = 92.0f;
			parameters_.umami = 95.0f;
			parameters_.volume = 80.0f;
			parameters_.temperature = 190.0f;
			parameters_.moisture = 15.0f;
			parameters_.burnt = 5.0f;
		}
		ImGui::SameLine();
		if (ImGui::Button("Nearly Burnt")) {
			parameters_.temperature = 220.0f;
			parameters_.moisture = 5.0f;
			parameters_.burnt = 45.0f;
		}

		ImGui::Text("Add Ingredients:");
		if (ImGui::Button("+ Pork")) AddIngredient(IngredientType::Pork);
		ImGui::SameLine();
		if (ImGui::Button("+ Beef")) AddIngredient(IngredientType::Beef);
		ImGui::SameLine();
		if (ImGui::Button("+ Chicken")) AddIngredient(IngredientType::Chicken);
		ImGui::SameLine();
		if (ImGui::Button("+ Egg")) AddIngredient(IngredientType::Egg);
		ImGui::SameLine();
		if (ImGui::Button("+ GreenOnion")) AddIngredient(IngredientType::GreenOnion);
		ImGui::SameLine();
		if (ImGui::Button("+ Seasoning")) AddIngredient(IngredientType::Seasoning);

		ImGui::Separator();

		// -----------------------------------------------------
		// 6. 炒飯評論家のリアルタイム判定プレビュー
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.8f, 1.0f), "=== Judge Evaluation Preview ===");
		auto eval = parameters_.Evaluate();
		const char* gradeStr = "";
		ImVec4 gradeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		switch (eval.grade)
		{
		case CookingParameters::JudgeGrade::Masterpiece:
			gradeStr = "MASTERPIECE (S Rank)";
			gradeColor = ImVec4(1.0f, 0.85f, 0.0f, 1.0f);
			break;
		case CookingParameters::JudgeGrade::Delicious:
			gradeStr = "DELICIOUS (A Rank)";
			gradeColor = ImVec4(0.2f, 1.0f, 0.3f, 1.0f);
			break;
		case CookingParameters::JudgeGrade::Acceptable:
			gradeStr = "ACCEPTABLE (B Rank)";
			gradeColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
			break;
		case CookingParameters::JudgeGrade::Unacceptable:
			gradeStr = "UNACCEPTABLE (Failed)";
			gradeColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
			break;
		}

		ImGui::TextColored(gradeColor, "Rank: %s (Score: %d/100)", gradeStr, eval.totalScore);
		ImGui::TextWrapped("Comment: \"%s\"", eval.commentEn.c_str());
		if (!eval.messageTextures.empty()) {
			ImGui::TextDisabled("Message Texture: %s", eval.messageTextures[0].c_str());
		}
	}
	ImGui::End();
#endif
}
