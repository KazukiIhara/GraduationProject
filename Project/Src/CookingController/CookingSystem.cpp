#include "CookingSystem.h"

#include <imgui.h>
#include <cmath>
#include <algorithm>

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
	parameters_.stirCount = 0;
	parameters_.addedIngredients.clear();
	parameters_.addedIngredients.push_back(IngredientType::Rice);

	heatExposureTimer_ = 0.0f;
	isHeating_ = false;
}

void CookingSystem::Update(
	float deltaTime,
	const LGF::Vector3& wokPosition,
	bool isWokShaking,
	float ladleStirStrength)
{
	if (deltaTime <= 0.0f) {
		return;
	}

	// 1. 火元による熱伝達と冷却
	UpdateHeat(deltaTime, wokPosition);

	// 2. 水分蒸発と焦げの進行
	UpdateMoistureAndBurnt(deltaTime, isWokShaking);

	// 3. 香りの自然拡散
	UpdateAroma(deltaTime);

	// 4. おたまによる継続的な炒め処理
	if (ladleStirStrength > 0.05f) {
		float stirAmount = ladleStirStrength * deltaTime;
		// 炒めることで具材が均一になり旨味UP
		parameters_.umami += 3.0f * stirAmount;
		// 水分が適正ならパラパラ度も微増
		if (parameters_.moisture < 50.0f && parameters_.temperature >= 100.0f) {
			parameters_.fluffiness += 1.5f * stirAmount;
		}
	}

	// パラメータを規定範囲内にクランプ
	parameters_.ClampAll();
}

void CookingSystem::OnWokShake(float shakeIntensity)
{
	parameters_.shakeCount++;
	heatExposureTimer_ = 0.0f; // 鍋底の熱を分散させて焦げを防止

	// 水分状態に応じたパラパラ度上昇効率
	// 水分が適度（10%～35%）で、ある程度温まっているときが一番パラパラになる
	float efficiency = 0.5f;
	if (parameters_.temperature >= 120.0f) {
		if (parameters_.moisture >= 10.0f && parameters_.moisture <= 35.0f) {
			efficiency = 2.0f; // 黄金ゾーン！
		} else if (parameters_.moisture > 35.0f) {
			efficiency = 0.8f; // まだベチャベチャ
		} else {
			efficiency = 0.3f; // 水分が飛びすぎてパラパラというよりパサパサ
		}
	}

	parameters_.fluffiness += 4.5f * shakeIntensity * efficiency;

	// 香りの立ち上がり（振ることで湯気・香気が広がる）
	if (parameters_.temperature >= 100.0f) {
		parameters_.aroma += 6.0f * shakeIntensity;
	}

	parameters_.ClampAll();
}

void CookingSystem::OnLadleStir(float stirIntensity)
{
	parameters_.stirCount++;

	// おたまによる撹拌：具材の調和（旨味UP）
	parameters_.umami += 2.0f * stirIntensity;

	// パラパラ度微増
	if (parameters_.temperature >= 90.0f) {
		parameters_.fluffiness += 1.0f * stirIntensity;
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
		item.volumeBonus = 25.0f;
		item.umamiBonus = 5.0f;
		item.aromaBonus = 2.0f;
		item.moistureBonus = 30.0f;
		break;
	case IngredientType::Egg:
		item.name = "溶き卵";
		item.volumeBonus = 10.0f;
		item.umamiBonus = 15.0f;
		item.aromaBonus = 10.0f;
		item.moistureBonus = 20.0f;
		break;
	case IngredientType::Pork:
		item.name = "豚肉（狩猟肉）";
		item.volumeBonus = 20.0f;
		item.umamiBonus = 25.0f;
		item.aromaBonus = 15.0f;
		item.moistureBonus = 10.0f;
		break;
	case IngredientType::Chicken:
		item.name = "鶏肉（狩猟肉）";
		item.volumeBonus = 15.0f;
		item.umamiBonus = 20.0f;
		item.aromaBonus = 12.0f;
		item.moistureBonus = 8.0f;
		break;
	case IngredientType::Beef:
		item.name = "牛肉（大型狩猟肉）";
		item.volumeBonus = 30.0f;
		item.umamiBonus = 35.0f;
		item.aromaBonus = 18.0f;
		item.moistureBonus = 12.0f;
		break;
	case IngredientType::GreenOnion:
		item.name = "刻みネギ";
		item.volumeBonus = 5.0f;
		item.umamiBonus = 10.0f;
		item.aromaBonus = 20.0f;
		item.moistureBonus = 5.0f;
		break;
	case IngredientType::Seasoning:
		item.name = "秘伝の調味料";
		item.volumeBonus = 0.0f;
		item.umamiBonus = 25.0f;
		item.aromaBonus = 15.0f;
		item.moistureBonus = 2.0f;
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

	// 冷たい具材が入ることで鍋の温度が少し低下する演出
	float coolingFromIngredient = ingredient.volumeBonus * 0.4f;
	parameters_.temperature = std::max(kRoomTemperature, parameters_.temperature - coolingFromIngredient);

	parameters_.ClampAll();
}

void CookingSystem::UpdateHeat(float deltaTime, const LGF::Vector3& wokPosition)
{
	isHeating_ = heatSource_.IsOverHeat(wokPosition);

	if (isHeating_) {
		float targetTemp = heatSource_.GetTargetMaxTemperature();
		float heatOutput = heatSource_.GetHeatOutput();

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
			parameters_.temperature -= (kCoolingRate + tempDiff * 0.03f) * deltaTime;
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

	// 100℃を超えると水分が蒸発
	if (parameters_.temperature >= 100.0f && parameters_.moisture > 0.0f) {
		float evapRate = (parameters_.temperature - 80.0f) * 0.04f;
		parameters_.moisture -= evapRate * deltaTime;
		if (parameters_.moisture < 0.0f) {
			parameters_.moisture = 0.0f;
		}
	}

	// 焦げの進行判定：
	// 高温（180℃以上）で水分が少なく、かつ一定時間振らずに火元で放置された場合
	if (isHeating_ && parameters_.temperature >= 170.0f && parameters_.moisture <= 15.0f) {
		if (heatExposureTimer_ > 2.5f) { // 2.5秒以上放置すると焦げ始める
			float burnRate = (heatExposureTimer_ - 2.5f) * 3.5f;
			parameters_.burnt += burnRate * deltaTime;

			// 焦げると香りが悪くなり、パラパラ度も劣化
			if (parameters_.burnt > 30.0f) {
				parameters_.aroma = std::max(0.0f, parameters_.aroma - 2.0f * deltaTime);
				parameters_.fluffiness = std::max(0.0f, parameters_.fluffiness - 1.5f * deltaTime);
			}
		}
	}
}

void CookingSystem::UpdateAroma(float deltaTime)
{
	// 香りは時間経過とともに大気中へゆっくり拡散・減衰（ただし動物を引き寄せるチャンス）
	if (parameters_.aroma > 0.0f) {
		parameters_.aroma -= 0.8f * deltaTime;
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
		// 1. スライド記載の5大パラメータ表示
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "=== 5 Big Parameters ===");

		// パラパラ度
		ImGui::ProgressBar(parameters_.fluffiness / 100.0f, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Fluffiness (Parapara): %.1f / 100", parameters_.fluffiness);

		// 香り（動物誘引）
		ImGui::ProgressBar(parameters_.aroma / 100.0f, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Aroma (Smell): %.1f / 100", parameters_.aroma);

		// ボリューム
		ImGui::ProgressBar(parameters_.volume / 100.0f, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Volume: %.1f / 100", parameters_.volume);

		// 温度
		ImGui::ProgressBar(parameters_.temperature / 300.0f, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Temperature: %.1f C", parameters_.temperature);

		// 旨味
		ImGui::ProgressBar(parameters_.umami / 100.0f, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Umami: %.1f / 100", parameters_.umami);

		ImGui::Separator();

		// -----------------------------------------------------
		// 2. 状態・物理補助パラメータ
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "=== State & Physics ===");
		ImGui::ProgressBar(parameters_.moisture / 100.0f, ImVec2(0.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Text("Moisture (Water): %.1f%%", parameters_.moisture);

		ImVec4 burntColor = (parameters_.burnt > 40.0f) ? ImVec4(1.0f, 0.2f, 0.2f, 1.0f) : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
		ImGui::TextColored(burntColor, "Burnt: %.1f%% | Overheat Timer: %.2f s", parameters_.burnt, heatExposureTimer_);
		ImGui::Text("Shakes: %d | Stirs: %d", parameters_.shakeCount, parameters_.stirCount);

		ImGui::Separator();

		// -----------------------------------------------------
		// 3. 火元（中華レンジ）コントロール
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "=== Stove (Heat Source) ===");
		ImGui::Text("Is Over Heat: [%s]", isHeating_ ? "HEATING (ON)" : "Off / Away");

		int currentFlame = static_cast<int>(heatSource_.GetFlameLevel());
		const char* flameNames[] = { "Off (Extinguished)", "Low", "Medium", "High (Strong Fire)" };
		if (ImGui::Combo("Stove Flame", &currentFlame, flameNames, IM_ARRAYSIZE(flameNames))) {
			heatSource_.SetFlameLevel(static_cast<HeatSource::FlameLevel>(currentFlame));
		}

		ImGui::Separator();

		// -----------------------------------------------------
		// 4. 手動テストアクション
		// -----------------------------------------------------
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "=== Action Simulation ===");
		if (ImGui::Button("Shake Wok (1x)")) {
			OnWokShake(1.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Ladle Stir (1x)")) {
			OnLadleStir(1.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Cooking")) {
			Reset();
		}

		ImGui::Text("Add Ingredients:");
		if (ImGui::Button("+ Pork (Hunting)")) {
			AddIngredient(IngredientType::Pork);
		}
		ImGui::SameLine();
		if (ImGui::Button("+ Beef (Hunting)")) {
			AddIngredient(IngredientType::Beef);
		}
		ImGui::SameLine();
		if (ImGui::Button("+ Chicken")) {
			AddIngredient(IngredientType::Chicken);
		}
		ImGui::SameLine();
		if (ImGui::Button("+ Egg")) {
			AddIngredient(IngredientType::Egg);
		}
		ImGui::SameLine();
		if (ImGui::Button("+ Green Onion")) {
			AddIngredient(IngredientType::GreenOnion);
		}
		ImGui::SameLine();
		if (ImGui::Button("+ Seasoning")) {
			AddIngredient(IngredientType::Seasoning);
		}

		ImGui::Separator();

		// -----------------------------------------------------
		// 5. 炒飯評論家のリアルタイム判定プレビュー
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
		ImGui::TextWrapped("Comment: \"%s\"", eval.comment.c_str());
	}
	ImGui::End();
#endif
}
