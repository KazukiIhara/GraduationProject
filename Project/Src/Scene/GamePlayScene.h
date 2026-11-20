#pragma once

#include "GameApp.h"
#include "LGF/Graphics/Skybox.h"
#include "LGF/Graphics/ToonLighting.h"
#include "LGF/Physics.h"

// Application
#include "Player/Player.h"
#include "CookingController/CookingController.h"
#include "Cooking/IngredientManager.h"
#include "Stage/StageManager.h"
#include "judge/JudgeManager.h"

// =========================================================
// ゲームプレイシーンクラス
// =========================================================
class GamePlayScene : public GameApp::Scene
{
public:
	explicit GamePlayScene(const InitData& init);
	~GamePlayScene() override;

	void Update() override;

	void Draw() const override;

	/// <summary>
	/// 入力によるカメラ更新処理
	/// </summary>
	void UpdateCamera();

#ifdef _DEBUG
	/// <summary>
	/// 画面効果のデバッグUIを描画
	/// </summary>
	void DrawScreenEffectDebugWindow();

	/// <summary>
	/// デバッグUIの設定をポストプロセスへ反映
	/// </summary>
	void ApplyScreenEffectSettings();
#endif

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

#ifdef _DEBUG
	struct ScreenEffectSettings {
		struct BloomSettings {
			bool enabled = false;
			float threshold = 1.0f;
			float softKnee = 0.5f;
			float intensity = 0.15f;
			float radius = 0.7f;
		};

		struct ColorAdjustmentSettings {
			bool enabled = false;
			float brightness = 0.0f;
			float contrast = 1.0f;
			float saturation = 1.0f;
			float intensity = 1.0f;
			LGF::Vector4 tint = LGF::Color::White;
		};

		struct LensDistortionSettings {
			bool enabled = false;
			float distortion = 0.15f;
			float scale = 1.0f;
			LGF::Vector2 center = {0.5f, 0.5f};
		};

		struct RadialBlurSettings {
			bool enabled = false;
			LGF::Vector2 center = {0.5f, 0.5f};
			float strength = 0.08f;
			float sampleCount = 12.0f;
		};

		struct VignetteSettings {
			bool enabled = false;
			float intensity = 0.25f;
			float smoothness = 0.5f;
			float roundness = 1.0f;
			LGF::Vector4 color = LGF::Color::Black;
		};

		struct SharpenSettings {
			bool enabled = false;
			float intensity = 0.5f;
			float threshold = 0.02f;
		};

		struct FxaaSettings {
			bool enabled = false;
			float edgeThreshold = 0.125f;
			float minimumEdgeThreshold = 0.0312f;
			float blendStrength = 0.75f;
		};

		struct OrderedDitheringSettings {
			bool enabled = false;
			float colorSteps = 8.0f;
			float intensity = 1.0f;
			float scale = 1.0f;
		};

		float exposureEV = 0.0f;
		BloomSettings bloom{};
		ColorAdjustmentSettings colorAdjustment{};
		bool grayscaleEnabled = false;
		float grayscaleIntensity = 1.0f;
		bool posterizationEnabled = false;
		float posterizationColorSteps = 8.0f;
		float posterizationIntensity = 1.0f;
		bool sepiaEnabled = false;
		float sepiaIntensity = 1.0f;
		bool colorInversionEnabled = false;
		float colorInversionIntensity = 1.0f;
		LensDistortionSettings lensDistortion{};
		RadialBlurSettings radialBlur{};
		bool chromaticAberrationEnabled = false;
		float chromaticAberrationIntensity = 2.0f;
		VignetteSettings vignette{};
		SharpenSettings sharpen{};
		FxaaSettings fxaa{};
		bool pixelationEnabled = false;
		float pixelSize = 8.0f;
		OrderedDitheringSettings orderedDithering{};
	};

	LGF::PostProcessProfileHandle postProcessProfile_{};
	ScreenEffectSettings screenEffectSettings_{};
#endif

	mutable LGF::DebugCamera3D camera_;
	LGF::DirectionalLight light_;
	LGF::ToonLighting toonLighting_;
	LGF::Grid3D grid_{ 20.0f, 1.0f };
	LGF::Physics::PhysicsWorld physicsWorld1_; // 炒飯と判定を取るものだけ
	LGF::Physics::PhysicsWorld physicsWorld2_; // それ以外（ステージ上のオブジェクトなど）
	LGF::Physics::StaticObjectHandle floorPhysicsHandle_{};
	LGF::InputCommand openFeatureShowcaseCommand_{ "Showcase.Open" };

	// スカイボックス
	LGF::Skybox skybox_{"../Assets/Textures/skybox.png"};

	// プレイヤー
	std::unique_ptr<Player> player_;
	// 調理器具マネージャー
	std::unique_ptr<CookingController> cookingController_;
	// 食材マネージャー
	std::unique_ptr<IngredientManager> ingredientManager_;
	// ステージオブジェクト管理クラス
	std::unique_ptr<StageManager> stageManager_;
	// 審査員マネージャー
	std::unique_ptr<JudgeManager> judgeManager_;

	// マウス入力による視点操作の有効状態
	bool isMouseCameraEnabled_ = false;
	// カメラのプレイヤー追従の有効状態
	bool isCameraFollowEnabled_ = false;
};

