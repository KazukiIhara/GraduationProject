#pragma once

#include "GameApp.h"

// Application
#include "CookingController/CookingController.h"
#include "judge/JudgeManager.h"

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
	mutable LGF::DebugCamera3D camera_;
	LGF::DirectionalLight light_;
	LGF::Grid3D grid_{ 20.0f, 1.0f };

	// モデル
	LGF::Model chineseRangeModel_{"../Assets/Models/chinese_range.gltf"}; // 中華レンジ

	// 調理器具マネージャー
	std::unique_ptr<CookingController> cookingController_;
	// 審査員マネージャー
	std::unique_ptr<JudgeManager> judgeManager_;
};

