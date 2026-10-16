#pragma once

#include "GameApp.h"

// Application
#include "Player/Player.h"
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

	/// <summary>
	/// 入力によるカメラ更新処理
	/// </summary>
	void UpdateCamera();

private:
	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kFixedCameraPosition_{0.0f, 10.0f, -25.0f}; // 固定カメラの位置
	const LGF::Vector3 kFixedCameraRotation_{0.9f, 0.0f, 0.0f}; // 固定カメラの回転

	const float kDeadZone = 0.15f; // スティックのデッドゾーン
	const float kStickSensitivity = 2.5f; // スティック感度
	const float kMouseSensitivity = 0.25f; // マウス感度
	const float kPitchLimit = 1.55f; // Pitchの回転制限

	// =========================================================
	// Member Variables
	// =========================================================

	mutable LGF::DebugCamera3D camera_;
	LGF::DirectionalLight light_;
	LGF::Grid3D grid_{ 20.0f, 1.0f };

	// モデル
	LGF::Model chineseRangeModel_{"../Assets/Models/chinese_range.gltf"}; // 中華レンジ

	// プレイヤー
	std::unique_ptr<Player> player_;
	// 調理器具マネージャー
	std::unique_ptr<CookingController> cookingController_;
	// 審査員マネージャー
	std::unique_ptr<JudgeManager> judgeManager_;
};

