#pragma once

#include "GameApp.h"
#include "LGF/Math/Vector3.h"

// =========================================================
// ゲームプレイシーンクラス
// =========================================================
class GamePlayScene : public GameApp::Scene
{
public:
	explicit GamePlayScene(const InitData& init);

	void Update() override;

	void Draw() const override;

private:
	enum class GameMode {
		Cooking, // 料理モード（前）
		Hunting  // 狩りモード（後ろ）
	};
	GameMode currentMode_ = GameMode::Cooking;

	mutable LGF::DebugCamera3D camera_;
	LGF::DirectionalLight light_;
	LGF::Grid3D grid_{ 20.0f, 1.0f };

	// モデル
	LGF::Model chineseRangeModel_{"../Assets/Models/chinese_range.gltf"}; // 中華レンジ
	LGF::Model wokModel_{ "../Assets/Models/wok.gltf" }; // 中華鍋
	LGF::Model ladlesModel_{ "../Assets/Models/ladles.gltf" }; // おたま
};

