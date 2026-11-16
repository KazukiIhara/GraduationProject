#include "CookingParameters.h"

#include <algorithm>
#include <cmath>

void CookingParameters::ClampAll()
{
	fluffiness = std::clamp(fluffiness, 0.0f, 100.0f);
	aroma = std::clamp(aroma, 0.0f, 100.0f);
	volume = std::clamp(volume, 0.0f, 100.0f);
	temperature = std::clamp(temperature, 0.0f, 300.0f);
	umami = std::clamp(umami, 0.0f, 100.0f);
	moisture = std::clamp(moisture, 0.0f, 100.0f);
	burnt = std::clamp(burnt, 0.0f, 100.0f);
}

CookingParameters::RadarChartData CookingParameters::GetRadarChartData() const
{
	RadarChartData norm{};
	norm.fluffiness = std::clamp(fluffiness / 100.0f, 0.0f, 1.0f);
	norm.aroma = std::clamp(aroma / 100.0f, 0.0f, 1.0f);
	norm.volume = std::clamp(volume / 100.0f, 0.0f, 1.0f);
	norm.umami = std::clamp(umami / 100.0f, 0.0f, 1.0f);

	// 温度適正度: 炒飯の適温（約150～210℃）で最大値1.0
	if (temperature < 60.0f) {
		norm.temperature = std::clamp(temperature / 120.0f, 0.0f, 0.5f); // 冷めている
	} else if (temperature <= 210.0f) {
		norm.temperature = 0.5f + (temperature - 60.0f) / 150.0f * 0.5f; // 適温ゾーン
	} else {
		norm.temperature = std::max(0.3f, 1.0f - (temperature - 210.0f) / 70.0f * 0.5f); // 熱すぎ
	}

	return norm;
}

CookingParameters::JudgeEvaluation CookingParameters::Evaluate(const JudgeConfig& config) const
{
	JudgeEvaluation eval{};
	eval.radarChart = GetRadarChartData();

	// 評価対象パラメータ: パラパラ度、旨味、ボリューム
	float score = (fluffiness * config.fluffinessWeight) +
	              (umami * config.umamiWeight) +
	              (volume * config.volumeWeight);

	// 焦げペナルティ
	if (burnt > config.burnPenaltyThreshold) {
		score -= (burnt - config.burnPenaltyThreshold) * config.burnPenaltyRate;
	}

	eval.totalScore = std::clamp(static_cast<int>(std::round(score)), 0, 100);

	// 判定とセリフ・テクスチャ（仮）
	if (burnt >= config.burntFailThreshold) {
		eval.grade = JudgeGrade::Unacceptable;
		eval.isAccepted = false;
		eval.comment = "丸焦げじゃないか！こんな炒飯、認められるか！";
		eval.commentEn = "Completely burnt! This is not acceptable at all!";
		eval.messageTextures = { "demoC1.png" };
	} else if (eval.totalScore >= config.scoreMasterpiece) {
		eval.grade = JudgeGrade::Masterpiece;
		eval.isAccepted = true;
		eval.comment = "こ、これは究極の炒飯…！やるじゃないか…！";
		eval.commentEn = "Incredible... this is the Ultimate Golden Fried Rice!";
		eval.messageTextures = { "demoA1.png", "demoA2.png" };
	} else if (eval.totalScore >= config.scoreDelicious) {
		eval.grade = JudgeGrade::Delicious;
		eval.isAccepted = true;
		eval.comment = "パラパラで旨味も十分、なかなかの腕前だな！";
		eval.commentEn = "Fluffy and rich in umami! Splendid cooking skill!";
		eval.messageTextures = { "demoA1.png" };
	} else if (eval.totalScore >= config.scoreAcceptable) {
		eval.grade = JudgeGrade::Acceptable;
		eval.isAccepted = true;
		eval.comment = "ふむ、悪くはないが…まだ改善の余地があるな。";
		eval.commentEn = "Not bad, but there is still room for improvement.";
		eval.messageTextures = { "demoB1.png" };
	} else {
		eval.grade = JudgeGrade::Unacceptable;
		eval.isAccepted = false;
		eval.comment = "こんな炒飯、認められるか！出直してこい！";
		eval.commentEn = "Unacceptable! Practice more and try again!";
		eval.messageTextures = { "demoC1.png" };
	}

	return eval;
}
