#include "CookingConfig.h"

#include "LGF/Parameter.h"

void CookingTuningConfig::ResetToDefault()
{
	*this = CookingTuningConfig{};
}

bool CookingTuningConfig::SaveToFile(const std::filesystem::path& path) const
{
	LGF::ParameterDocument doc;

	// 1. 熱・温度
	doc.Set("Heat/heatOutput", heatOutput);
	doc.Set("Heat/targetMaxTemperature", targetMaxTemperature);
	doc.Set("Heat/naturalCoolingRate", naturalCoolingRate);
	doc.Set("Heat/roomTemperature", roomTemperature);
	doc.Set("Heat/tempDiffCoolingCoeff", tempDiffCoolingCoeff);

	// 2. 水分蒸発・焦げ
	doc.Set("MoistureBurnt/evapRateCoeff", evapRateCoeff);
	doc.Set("MoistureBurnt/residualEvapMultiplier", residualEvapMultiplier);
	doc.Set("MoistureBurnt/burnStartTemp", burnStartTemp);
	doc.Set("MoistureBurnt/burnMaxMoisture", burnMaxMoisture);
	doc.Set("MoistureBurnt/burnGraceTime", burnGraceTime);
	doc.Set("MoistureBurnt/burnRateCoeff", burnRateCoeff);

	// 3. 鍋振りアクション
	doc.Set("WokShake/fluffinessVertical", shakeFluffinessVertical);
	doc.Set("WokShake/aromaVertical", shakeAromaVertical);
	doc.Set("WokShake/moistureLossVertical", shakeMoistureLossVertical);
	doc.Set("WokShake/coolingVertical", shakeCoolingVertical);
	doc.Set("WokShake/umamiHorizontal", shakeUmamiHorizontal);
	doc.Set("WokShake/fluffinessHorizontal", shakeFluffinessHorizontal);
	doc.Set("WokShake/coolingHorizontal", shakeCoolingHorizontal);

	doc.Set("WokShake/goldenZoneTempMin", goldenZoneTempMin);
	doc.Set("WokShake/goldenZoneMoistMin", goldenZoneMoistMin);
	doc.Set("WokShake/goldenZoneMoistMax", goldenZoneMoistMax);
	doc.Set("WokShake/goldenZoneEfficiency", goldenZoneEfficiency);
	doc.Set("WokShake/wetZoneEfficiency", wetZoneEfficiency);
	doc.Set("WokShake/dryZoneEfficiency", dryZoneEfficiency);

	doc.Set("WokShake/strengthWeakCoeff", strengthWeakCoeff);
	doc.Set("WokShake/strengthMediumCoeff", strengthMediumCoeff);
	doc.Set("WokShake/strengthStrongCoeff", strengthStrongCoeff);

	// 4. おたまアクション
	doc.Set("Ladle/umamiRate", ladleUmamiRate);
	doc.Set("Ladle/fluffinessRate", ladleFluffinessRate);
	doc.Set("Ladle/stirUmamiInstant", ladleStirUmamiInstant);
	doc.Set("Ladle/stirFluffinessInstant", ladleStirFluffinessInstant);
	doc.Set("Ladle/swingSpeedScale", ladleSwingSpeedScale);
	doc.Set("Ladle/swingThreshold", ladleSwingThreshold);

	// 5. 香り・動物誘引
	doc.Set("Aroma/decayRate", aromaDecayRate);
	doc.Set("Aroma/attractMaxRadius", attractMaxRadius);

	// 6. 初期値
	doc.Set("Initial/fluffiness", initFluffiness);
	doc.Set("Initial/aroma", initAroma);
	doc.Set("Initial/volume", initVolume);
	doc.Set("Initial/temperature", initTemperature);
	doc.Set("Initial/umami", initUmami);
	doc.Set("Initial/moisture", initMoisture);

	// 7. 食材ボーナス
	auto setBonus = [&](const std::string& prefix, const IngredientBonus& b) {
		doc.Set(prefix + "/volume", b.volume);
		doc.Set(prefix + "/umami", b.umami);
		doc.Set(prefix + "/aroma", b.aroma);
		doc.Set(prefix + "/moisture", b.moisture);
	};
	setBonus("Ingredients/Rice", ingRice);
	setBonus("Ingredients/Egg", ingEgg);
	setBonus("Ingredients/Pork", ingPork);
	setBonus("Ingredients/Chicken", ingChicken);
	setBonus("Ingredients/Beef", ingBeef);
	setBonus("Ingredients/GreenOnion", ingGreenOnion);
	setBonus("Ingredients/Seasoning", ingSeasoning);

	// 8. 審査員判定
	doc.Set("Judge/fluffinessWeight", judge.fluffinessWeight);
	doc.Set("Judge/umamiWeight", judge.umamiWeight);
	doc.Set("Judge/volumeWeight", judge.volumeWeight);
	doc.Set("Judge/burnPenaltyThreshold", judge.burnPenaltyThreshold);
	doc.Set("Judge/burnPenaltyRate", judge.burnPenaltyRate);
	doc.Set("Judge/burntFailThreshold", judge.burntFailThreshold);
	doc.Set("Judge/scoreMasterpiece", static_cast<int32_t>(judge.scoreMasterpiece));
	doc.Set("Judge/scoreDelicious", static_cast<int32_t>(judge.scoreDelicious));
	doc.Set("Judge/scoreAcceptable", static_cast<int32_t>(judge.scoreAcceptable));

	return doc.Save(path).succeeded;
}

bool CookingTuningConfig::LoadFromFile(const std::filesystem::path& path)
{
	LGF::ParameterDocument doc;
	auto res = doc.Load(path);
	if (!res.succeeded) {
		return false;
	}

	// 1. 熱・温度
	heatOutput = doc.GetOr<float>("Heat/heatOutput", heatOutput);
	targetMaxTemperature = doc.GetOr<float>("Heat/targetMaxTemperature", targetMaxTemperature);
	naturalCoolingRate = doc.GetOr<float>("Heat/naturalCoolingRate", naturalCoolingRate);
	roomTemperature = doc.GetOr<float>("Heat/roomTemperature", roomTemperature);
	tempDiffCoolingCoeff = doc.GetOr<float>("Heat/tempDiffCoolingCoeff", tempDiffCoolingCoeff);

	// 2. 水分蒸発・焦げ
	evapRateCoeff = doc.GetOr<float>("MoistureBurnt/evapRateCoeff", evapRateCoeff);
	residualEvapMultiplier = doc.GetOr<float>("MoistureBurnt/residualEvapMultiplier", residualEvapMultiplier);
	burnStartTemp = doc.GetOr<float>("MoistureBurnt/burnStartTemp", burnStartTemp);
	burnMaxMoisture = doc.GetOr<float>("MoistureBurnt/burnMaxMoisture", burnMaxMoisture);
	burnGraceTime = doc.GetOr<float>("MoistureBurnt/burnGraceTime", burnGraceTime);
	burnRateCoeff = doc.GetOr<float>("MoistureBurnt/burnRateCoeff", burnRateCoeff);

	// 3. 鍋振りアクション
	shakeFluffinessVertical = doc.GetOr<float>("WokShake/fluffinessVertical", shakeFluffinessVertical);
	shakeAromaVertical = doc.GetOr<float>("WokShake/aromaVertical", shakeAromaVertical);
	shakeMoistureLossVertical = doc.GetOr<float>("WokShake/moistureLossVertical", shakeMoistureLossVertical);
	shakeCoolingVertical = doc.GetOr<float>("WokShake/coolingVertical", shakeCoolingVertical);
	shakeUmamiHorizontal = doc.GetOr<float>("WokShake/umamiHorizontal", shakeUmamiHorizontal);
	shakeFluffinessHorizontal = doc.GetOr<float>("WokShake/fluffinessHorizontal", shakeFluffinessHorizontal);
	shakeCoolingHorizontal = doc.GetOr<float>("WokShake/coolingHorizontal", shakeCoolingHorizontal);

	goldenZoneTempMin = doc.GetOr<float>("WokShake/goldenZoneTempMin", goldenZoneTempMin);
	goldenZoneMoistMin = doc.GetOr<float>("WokShake/goldenZoneMoistMin", goldenZoneMoistMin);
	goldenZoneMoistMax = doc.GetOr<float>("WokShake/goldenZoneMoistMax", goldenZoneMoistMax);
	goldenZoneEfficiency = doc.GetOr<float>("WokShake/goldenZoneEfficiency", goldenZoneEfficiency);
	wetZoneEfficiency = doc.GetOr<float>("WokShake/wetZoneEfficiency", wetZoneEfficiency);
	dryZoneEfficiency = doc.GetOr<float>("WokShake/dryZoneEfficiency", dryZoneEfficiency);

	strengthWeakCoeff = doc.GetOr<float>("WokShake/strengthWeakCoeff", strengthWeakCoeff);
	strengthMediumCoeff = doc.GetOr<float>("WokShake/strengthMediumCoeff", strengthMediumCoeff);
	strengthStrongCoeff = doc.GetOr<float>("WokShake/strengthStrongCoeff", strengthStrongCoeff);

	// 4. おたまアクション
	ladleUmamiRate = doc.GetOr<float>("Ladle/umamiRate", ladleUmamiRate);
	ladleFluffinessRate = doc.GetOr<float>("Ladle/fluffinessRate", ladleFluffinessRate);
	ladleStirUmamiInstant = doc.GetOr<float>("Ladle/stirUmamiInstant", ladleStirUmamiInstant);
	ladleStirFluffinessInstant = doc.GetOr<float>("Ladle/stirFluffinessInstant", ladleStirFluffinessInstant);
	ladleSwingSpeedScale = doc.GetOr<float>("Ladle/swingSpeedScale", ladleSwingSpeedScale);
	ladleSwingThreshold = doc.GetOr<float>("Ladle/swingThreshold", ladleSwingThreshold);

	// 5. 香り・動物誘引
	aromaDecayRate = doc.GetOr<float>("Aroma/decayRate", aromaDecayRate);
	attractMaxRadius = doc.GetOr<float>("Aroma/attractMaxRadius", attractMaxRadius);

	// 6. 初期値
	initFluffiness = doc.GetOr<float>("Initial/fluffiness", initFluffiness);
	initAroma = doc.GetOr<float>("Initial/aroma", initAroma);
	initVolume = doc.GetOr<float>("Initial/volume", initVolume);
	initTemperature = doc.GetOr<float>("Initial/temperature", initTemperature);
	initUmami = doc.GetOr<float>("Initial/umami", initUmami);
	initMoisture = doc.GetOr<float>("Initial/moisture", initMoisture);

	// 7. 食材ボーナス
	auto getBonus = [&](const std::string& prefix, IngredientBonus& b) {
		b.volume = doc.GetOr<float>(prefix + "/volume", b.volume);
		b.umami = doc.GetOr<float>(prefix + "/umami", b.umami);
		b.aroma = doc.GetOr<float>(prefix + "/aroma", b.aroma);
		b.moisture = doc.GetOr<float>(prefix + "/moisture", b.moisture);
	};
	getBonus("Ingredients/Rice", ingRice);
	getBonus("Ingredients/Egg", ingEgg);
	getBonus("Ingredients/Pork", ingPork);
	getBonus("Ingredients/Chicken", ingChicken);
	getBonus("Ingredients/Beef", ingBeef);
	getBonus("Ingredients/GreenOnion", ingGreenOnion);
	getBonus("Ingredients/Seasoning", ingSeasoning);

	// 8. 審査員判定
	judge.fluffinessWeight = doc.GetOr<float>("Judge/fluffinessWeight", judge.fluffinessWeight);
	judge.umamiWeight = doc.GetOr<float>("Judge/umamiWeight", judge.umamiWeight);
	judge.volumeWeight = doc.GetOr<float>("Judge/volumeWeight", judge.volumeWeight);
	judge.burnPenaltyThreshold = doc.GetOr<float>("Judge/burnPenaltyThreshold", judge.burnPenaltyThreshold);
	judge.burnPenaltyRate = doc.GetOr<float>("Judge/burnPenaltyRate", judge.burnPenaltyRate);
	judge.burntFailThreshold = doc.GetOr<float>("Judge/burntFailThreshold", judge.burntFailThreshold);
	judge.scoreMasterpiece = doc.GetOr<int32_t>("Judge/scoreMasterpiece", judge.scoreMasterpiece);
	judge.scoreDelicious = doc.GetOr<int32_t>("Judge/scoreDelicious", judge.scoreDelicious);
	judge.scoreAcceptable = doc.GetOr<int32_t>("Judge/scoreAcceptable", judge.scoreAcceptable);

	return true;
}
