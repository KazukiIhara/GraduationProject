#include "GamePlayScene.h"

#include <imgui.h>

#include "LGF/Input.h"
#include "LGF/System.h"

#include <algorithm>

#include "Collision/CollisionManager.h"
#include "Input/XboxController.h"

using namespace LGF;

namespace {

Physics::PhysicsWorldSettings MakePhysicsWorldSettings() {
	Physics::PhysicsWorldSettings settings{};
	settings.worldBounds = {
	    .min = {-100.0f, -10.0f, -100.0f},
	    .max = {100.0f,  20.0f,  100.0f },
	};
	settings.substepCount = 8u;
	settings.solverIterationCount = 8u;
	settings.maximumParticleCount = 4096u;
	settings.maximumColliderCount = 256u;
	settings.maximumParticleCommandCount = 16384u;
	settings.linearDamping = 0.2f;
	return settings;
}

} // namespace

GamePlayScene::GamePlayScene(const InitData& init)
    : IScene(init), camera_(15.0f), light_({0.5f, -1.0f, 0.35f}, Color::White, 1.5f), physicsWorld1_(MakePhysicsWorldSettings()), physicsWorld2_(MakePhysicsWorldSettings()) {

	camera_.translation = {0.0f, 7.0f, -20.0f};
	camera_.yaw = 0.0f;
	camera_.pitch = 0.3f;

	/* コリジョンマネージャー設定 */
	auto* collisionManager = CollisionManager::GetInstance();
	// 一応クリア
	collisionManager->Clear();
	// タグの登録
	collisionManager->RegisterTag("Player");            // プレイヤー
	collisionManager->RegisterTag("Solid");             // 固体（プレイヤーとの押し戻しを行うオブジェクト）
	collisionManager->RegisterTag("InteractionSensor"); // インタラクト用センサー（プレイヤーの中華レンジ周辺でのモード切替用）

	player_ = std::make_unique<Player>(physicsWorld2_); // 炒飯と判定を取らないほうのワールドに設定（ステージオブジェクトなど）

	floorPhysicsHandle_ = physicsWorld1_.AddStaticObject(
	    Collision::AABB{
	        .min = {-20.0f, -0.5f, -20.0f},
	        .max = {20.0f,  0.0f,  20.0f },
		},
	    Physics::StaticObjectOptions{
	        .material = {.restitution = 0.05f, .friction = 0.9f},
	        .objectHandle = {1u, 1u},
	   });

	cookingController_ = std::make_unique<CookingController>(physicsWorld1_);
	ingredientManager_ = std::make_unique<IngredientManager>(physicsWorld1_);

	stageManager_ = std::make_unique<StageManager>("../Assets/Stages/stage.json", physicsWorld1_, physicsWorld2_);

	judgeManager_ = std::make_unique<JudgeManager>();

#ifdef _DEBUG
	postProcessProfile_ = Graphics3D::CreatePostProcessProfile("Default.json");
	if (Graphics3D::IsValid(postProcessProfile_)) {
		Graphics3D::SetPostProcessProfile(postProcessProfile_);
		ApplyScreenEffectSettings();
	}
#endif
}

GamePlayScene::~GamePlayScene() {
#ifdef _DEBUG
	if (Graphics3D::IsValid(postProcessProfile_)) {
		Graphics3D::ResetPostProcessProfile();
		Graphics3D::RemovePostProcessProfile(postProcessProfile_);
	}
#endif
}

void GamePlayScene::Update() {
	if (openUIWorldDemoCommand_.Trigger()) {
		GetData().previousScene = GetState();
		ChangeScene(SceneNames::UIWorldDemo);
		return;
	}
	if (Key::L.Trigger()) {
		ChangeScene(SceneNames::ColliderWorldDemo);
		return;
	}
	if (Key::P.Trigger()) {
		ChangeScene(SceneNames::PhysicsWorldDemo);
		return;
	}
	if (Key::K.Trigger()) {
		ChangeScene(SceneNames::SkinningAnimationDemo);
		return;
	}

	/* カメラ操作 */
	static bool prevCookingMode = false;
	bool currentCookingMode = player_->IsCookingMode();

	if (!player_->IsCookingMode()) {
		// 料理モード中でなければ追従カメラ
		UpdateCamera();

		// 料理モードが解除された瞬間、pitchを調整
		if (prevCookingMode && !currentCookingMode) {
			camera_.pitch = 0.3f;
		}
	} else {
		// 料理モード中はカメラ固定
		camera_.translation = {0.0f, 4.7f, -2.6f};
		camera_.yaw = 0.0f;
		camera_.pitch = 0.7f;
	}
	prevCookingMode = currentCookingMode;

	/* プレイヤー更新 */
	player_->Update(camera_.yaw);
	// カメラの位置をプレイヤーに追従させる
	if (Key::Num1.Trigger()) { // Num1キーで有効状態切替
		isCameraFollowEnabled_ = !isCameraFollowEnabled_;
	}
	if (isCameraFollowEnabled_) {
		camera_.translation = player_->GetEyePosition();
	}
	/* 調理器具マネージャーの更新 */
	cookingController_->Update();
	/* 食材マネージャーの更新 */
	ingredientManager_->Update(cookingController_->GetWok().GetRiceSpawnPosition());
	/* 物理ワールドの更新 */
	physicsWorld1_.Update(static_cast<float>(System::DeltaTime()));
	physicsWorld2_.Update(static_cast<float>(System::DeltaTime()));
	/* ステージオブジェクト管理クラスの更新 */
	stageManager_->Update();
	// 審査員マネージャーの更新
	judgeManager_->Update(player_.get(), camera_);

	/* コリジョンマネージャー更新 */
	CollisionManager::GetInstance()->Step();

#ifdef _DEBUG
	if (ImGui::Begin("GamePlay Debug")) {

		ImGui::Text("fps : %.2f", ImGui::GetIO().Framerate);
		ImGui::Separator();

		ImGui::Text("Num1 : Toggle Camera Follow");
		ImGui::SameLine();
		ImGui::Text("[%s]", isCameraFollowEnabled_ ? "Enabled" : "Disabled");

		// 現在の設定状態
		ImGui::Text("Num2 : Toggle Mouse Camera");
		ImGui::SameLine();
		ImGui::Text("[%s]", isMouseCameraEnabled_ ? "Enabled" : "Disabled");
		ImGui::Text("K : Open Skinning Animation Demo");

		ImGui::Separator();

		// カメラ情報
		ImGui::DragFloat3("Camera Position", &camera_.translation.x, 0.05f);
		ImGui::DragFloat("Camera Yaw", &camera_.yaw, 0.01f, -6.28f, 6.28f);
		ImGui::DragFloat("Camera Pitch", &camera_.pitch, 0.01f, -1.57f, 1.57f);
	}
	ImGui::End();

	player_->DrawImGuiDebug();
	cookingController_->DrawImGuiDebug();
	ingredientManager_->DrawImGuiDebug();
	stageManager_->DrawImGuiDebug();

	CollisionManager::GetInstance()->DrawImGuiDebug();
	DrawScreenEffectDebugWindow();
#endif
}

void GamePlayScene::Draw() const {
	skybox_.SetScene();

	camera_.SetScene();
	light_.SetScene();
	toonLighting_.SetScene();

	// プレイヤー描画
	player_->Draw();
	// 調理器具マネージャーの描画
	cookingController_->Draw();
	// 食材マネージャーの描画
	ingredientManager_->Draw();
	// ステージオブジェクト管理クラスの描画
	stageManager_->Draw();
	// 審査員マネージャーの描画
	judgeManager_->Draw();
}

void GamePlayScene::UpdateCamera() {
	float yawDelta = 0.0f;
	float pitchDelta = 0.0f;
	const float deltaTime = static_cast<float>(System::DeltaTime());

	auto processStickInput = [&](const Gamepad& gamepad) {
		Vector2 rightStick = gamepad.RightStick();

		// デッドゾーン処理
		if (std::abs(rightStick.x) < kDeadZone)
			rightStick.x = 0.0f;
		if (std::abs(rightStick.y) < kDeadZone)
			rightStick.y = 0.0f;

		yawDelta += rightStick.x * kStickSensitivity * deltaTime;
		pitchDelta -= rightStick.y * kStickSensitivity * deltaTime;
	};

	/* 右スティックによる視点操作 */
	auto rightJoyCon = JoyCon::Right();
	if (rightJoyCon.has_value() && rightJoyCon->IsConnected()) {
		processStickInput(*rightJoyCon);
	}

	const auto xboxController = XboxController::Get();
	if (xboxController.has_value()) {
		processStickInput(*xboxController);
	}

	/* マウスによる視点操作（デバッグ） */
	if (Key::Num2.Trigger()) { // Num2キーで入力有効状態切替
		isMouseCameraEnabled_ = !isMouseCameraEnabled_;
	}
	if (isMouseCameraEnabled_) {
		Vector2 mouseDelta = Mouse::Delta();
		yawDelta += mouseDelta.x * kMouseSensitivity * deltaTime;
		pitchDelta += mouseDelta.y * kMouseSensitivity * deltaTime;
	}

	cookingController_->DrawImGuiDebug();
	judgeManager_->DrawImGuiDebug();

	// 最終的なカメラ角度に反映
	camera_.yaw += yawDelta;
	camera_.pitch += pitchDelta;

	// Pitchの回転制限
	camera_.pitch = std::clamp(camera_.pitch, -kPitchLimit, kPitchLimit);
}

#ifdef _DEBUG
void GamePlayScene::DrawScreenEffectDebugWindow() {
	ImGui::SetNextWindowSize(ImVec2(420.0f, 650.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("GamePlay Screen Effects")) {
		if (!Graphics3D::IsValid(postProcessProfile_)) {
			ImGui::TextDisabled("Post process profile could not be loaded.");
			ImGui::End();
			return;
		}

		if (ImGui::Button("Reset Post Effects")) {
			screenEffectSettings_ = {};
			Graphics3D::ClearPostProcessOverrides(postProcessProfile_);
		}

		if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderFloat("Exposure EV", &screenEffectSettings_.exposureEV, -10.0f, 10.0f);
			ImGui::DragFloat3("Light Direction", &light_.direction.x, 0.01f, -1.0f, 1.0f);
			ImGui::ColorEdit3("Light Color", &light_.color.x);
			ImGui::SliderFloat("Light Intensity", &light_.intensity, 0.0f, 10.0f);
			ImGui::SliderFloat("Skybox Intensity", &skybox_.intensity, 0.0f, 10.0f);
			ImGui::SliderAngle("Skybox Rotation", &skybox_.rotation, -180.0f, 180.0f);
		}

		if (ImGui::CollapsingHeader("Toon Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Reset Toon Lighting")) {
				toonLighting_ = {};
			}
			ImGui::Checkbox("Enabled##ToonLighting", &toonLighting_.enabled);
			ImGui::ColorEdit3("Shadow Color##ToonLighting", &toonLighting_.shadowColor.x);
			ImGui::ColorEdit3("Middle Color##ToonLighting", &toonLighting_.middleColor.x);
			ImGui::ColorEdit3("Light Color##ToonLighting", &toonLighting_.lightColor.x);
			ImGui::SliderFloat("Shadow Threshold", &toonLighting_.shadowThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("Light Threshold", &toonLighting_.lightThreshold, toonLighting_.shadowThreshold, 1.0f);
			ImGui::SliderFloat("Smoothness##ToonLighting", &toonLighting_.smoothness, 0.0f, 0.5f);
		}

		if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Enabled##Bloom", &screenEffectSettings_.bloom.enabled);
			ImGui::SliderFloat("Threshold##Bloom", &screenEffectSettings_.bloom.threshold, 0.0f, 10.0f);
			ImGui::SliderFloat("Soft Knee##Bloom", &screenEffectSettings_.bloom.softKnee, 0.0f, 1.0f);
			ImGui::SliderFloat("Intensity##Bloom", &screenEffectSettings_.bloom.intensity, 0.0f, 5.0f);
			ImGui::SliderFloat("Radius##Bloom", &screenEffectSettings_.bloom.radius, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Color Adjustment")) {
			ImGui::Checkbox("Enabled##ColorAdjustment", &screenEffectSettings_.colorAdjustment.enabled);
			ImGui::SliderFloat("Brightness", &screenEffectSettings_.colorAdjustment.brightness, -1.0f, 1.0f);
			ImGui::SliderFloat("Contrast", &screenEffectSettings_.colorAdjustment.contrast, 0.0f, 2.0f);
			ImGui::SliderFloat("Saturation", &screenEffectSettings_.colorAdjustment.saturation, 0.0f, 2.0f);
			ImGui::SliderFloat("Intensity##ColorAdjustment", &screenEffectSettings_.colorAdjustment.intensity, 0.0f, 1.0f);
			ImGui::ColorEdit4("Tint", &screenEffectSettings_.colorAdjustment.tint.x, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
		}

		if (ImGui::CollapsingHeader("Color Filters")) {
			ImGui::Checkbox("Grayscale", &screenEffectSettings_.grayscaleEnabled);
			ImGui::SliderFloat("Intensity##Grayscale", &screenEffectSettings_.grayscaleIntensity, 0.0f, 1.0f);
			ImGui::Checkbox("Sepia", &screenEffectSettings_.sepiaEnabled);
			ImGui::SliderFloat("Intensity##Sepia", &screenEffectSettings_.sepiaIntensity, 0.0f, 1.0f);
			ImGui::Checkbox("Color Inversion", &screenEffectSettings_.colorInversionEnabled);
			ImGui::SliderFloat("Intensity##ColorInversion", &screenEffectSettings_.colorInversionIntensity, 0.0f, 1.0f);
			ImGui::Checkbox("Posterization", &screenEffectSettings_.posterizationEnabled);
			ImGui::SliderFloat("Color Steps##Posterization", &screenEffectSettings_.posterizationColorSteps, 2.0f, 64.0f, "%.0f");
			ImGui::SliderFloat("Intensity##Posterization", &screenEffectSettings_.posterizationIntensity, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Lens Effects")) {
			ImGui::Checkbox("Lens Distortion", &screenEffectSettings_.lensDistortion.enabled);
			ImGui::SliderFloat("Distortion", &screenEffectSettings_.lensDistortion.distortion, -1.0f, 1.0f);
			ImGui::SliderFloat("Scale##LensDistortion", &screenEffectSettings_.lensDistortion.scale, 0.5f, 2.0f);
			ImGui::SliderFloat2("Center##LensDistortion", &screenEffectSettings_.lensDistortion.center.x, 0.0f, 1.0f);
			ImGui::Checkbox("Chromatic Aberration", &screenEffectSettings_.chromaticAberrationEnabled);
			ImGui::SliderFloat("Intensity##ChromaticAberration", &screenEffectSettings_.chromaticAberrationIntensity, 0.0f, 32.0f);
			ImGui::Checkbox("Vignette", &screenEffectSettings_.vignette.enabled);
			ImGui::SliderFloat("Intensity##Vignette", &screenEffectSettings_.vignette.intensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Smoothness##Vignette", &screenEffectSettings_.vignette.smoothness, 0.001f, 1.0f);
			ImGui::SliderFloat("Roundness##Vignette", &screenEffectSettings_.vignette.roundness, 0.0f, 1.0f);
			ImGui::ColorEdit4("Color##Vignette", &screenEffectSettings_.vignette.color.x);
		}

		if (ImGui::CollapsingHeader("Blur / Sharpen")) {
			ImGui::Checkbox("Radial Blur", &screenEffectSettings_.radialBlur.enabled);
			ImGui::SliderFloat2("Center##RadialBlur", &screenEffectSettings_.radialBlur.center.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Strength##RadialBlur", &screenEffectSettings_.radialBlur.strength, -0.25f, 0.25f);
			ImGui::SliderFloat("Sample Count##RadialBlur", &screenEffectSettings_.radialBlur.sampleCount, 2.0f, 32.0f, "%.0f");
			ImGui::Checkbox("Sharpen", &screenEffectSettings_.sharpen.enabled);
			ImGui::SliderFloat("Intensity##Sharpen", &screenEffectSettings_.sharpen.intensity, 0.0f, 2.0f);
			ImGui::SliderFloat("Threshold##Sharpen", &screenEffectSettings_.sharpen.threshold, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Pixel Effects / Anti-Aliasing")) {
			ImGui::Checkbox("FXAA", &screenEffectSettings_.fxaa.enabled);
			ImGui::SliderFloat("Edge Threshold##FXAA", &screenEffectSettings_.fxaa.edgeThreshold, 0.0312f, 0.333f);
			ImGui::SliderFloat("Minimum Edge Threshold##FXAA", &screenEffectSettings_.fxaa.minimumEdgeThreshold, 0.0f, 0.0833f);
			ImGui::SliderFloat("Blend Strength##FXAA", &screenEffectSettings_.fxaa.blendStrength, 0.0f, 1.0f);
			ImGui::Checkbox("Pixelation", &screenEffectSettings_.pixelationEnabled);
			ImGui::SliderFloat("Pixel Size", &screenEffectSettings_.pixelSize, 1.0f, 128.0f, "%.0f");
			ImGui::Checkbox("Ordered Dithering", &screenEffectSettings_.orderedDithering.enabled);
			ImGui::SliderFloat("Color Steps##OrderedDithering", &screenEffectSettings_.orderedDithering.colorSteps, 2.0f, 64.0f, "%.0f");
			ImGui::SliderFloat("Intensity##OrderedDithering", &screenEffectSettings_.orderedDithering.intensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Scale##OrderedDithering", &screenEffectSettings_.orderedDithering.scale, 1.0f, 8.0f);
		}
	}
	ImGui::End();

	ApplyScreenEffectSettings();
}

void GamePlayScene::ApplyScreenEffectSettings() {
	if (!Graphics3D::IsValid(postProcessProfile_)) {
		return;
	}

	const auto setEnabled = [this](const char* effectId, bool enabled) {
		Graphics3D::SetPostProcessEffectEnabled(postProcessProfile_, effectId, enabled);
	};
	const auto setFloat = [this](const char* effectId, const char* parameterName, float value) {
		Graphics3D::SetPostProcessParameter(postProcessProfile_, effectId, parameterName, value);
	};

	Graphics3D::SetToneMappingParameter(postProcessProfile_, "ExposureEV", screenEffectSettings_.exposureEV);

	setEnabled("MainBloom", screenEffectSettings_.bloom.enabled);
	setFloat("MainBloom", "Threshold", screenEffectSettings_.bloom.threshold);
	setFloat("MainBloom", "SoftKnee", screenEffectSettings_.bloom.softKnee);
	setFloat("MainBloom", "Intensity", screenEffectSettings_.bloom.intensity);
	setFloat("MainBloom", "Radius", screenEffectSettings_.bloom.radius);

	setEnabled("SceneColor", screenEffectSettings_.colorAdjustment.enabled);
	setFloat("SceneColor", "Brightness", screenEffectSettings_.colorAdjustment.brightness);
	setFloat("SceneColor", "Contrast", screenEffectSettings_.colorAdjustment.contrast);
	setFloat("SceneColor", "Saturation", screenEffectSettings_.colorAdjustment.saturation);
	setFloat("SceneColor", "Intensity", screenEffectSettings_.colorAdjustment.intensity);
	Graphics3D::SetPostProcessParameter(postProcessProfile_, "SceneColor", "Tint", screenEffectSettings_.colorAdjustment.tint);

	setEnabled("Grayscale", screenEffectSettings_.grayscaleEnabled);
	setFloat("Grayscale", "Intensity", screenEffectSettings_.grayscaleIntensity);
	setEnabled("Posterization", screenEffectSettings_.posterizationEnabled);
	setFloat("Posterization", "ColorSteps", screenEffectSettings_.posterizationColorSteps);
	setFloat("Posterization", "Intensity", screenEffectSettings_.posterizationIntensity);
	setEnabled("Sepia", screenEffectSettings_.sepiaEnabled);
	setFloat("Sepia", "Intensity", screenEffectSettings_.sepiaIntensity);
	setEnabled("ColorInversion", screenEffectSettings_.colorInversionEnabled);
	setFloat("ColorInversion", "Intensity", screenEffectSettings_.colorInversionIntensity);

	setEnabled("LensDistortion", screenEffectSettings_.lensDistortion.enabled);
	setFloat("LensDistortion", "Distortion", screenEffectSettings_.lensDistortion.distortion);
	setFloat("LensDistortion", "Scale", screenEffectSettings_.lensDistortion.scale);
	Graphics3D::SetPostProcessParameter(postProcessProfile_, "LensDistortion", "Center", screenEffectSettings_.lensDistortion.center);
	setEnabled("RadialBlur", screenEffectSettings_.radialBlur.enabled);
	Graphics3D::SetPostProcessParameter(postProcessProfile_, "RadialBlur", "Center", screenEffectSettings_.radialBlur.center);
	setFloat("RadialBlur", "Strength", screenEffectSettings_.radialBlur.strength);
	setFloat("RadialBlur", "SampleCount", screenEffectSettings_.radialBlur.sampleCount);
	setEnabled("ChromaticAberration", screenEffectSettings_.chromaticAberrationEnabled);
	setFloat("ChromaticAberration", "Intensity", screenEffectSettings_.chromaticAberrationIntensity);

	setEnabled("Vignette", screenEffectSettings_.vignette.enabled);
	setFloat("Vignette", "Intensity", screenEffectSettings_.vignette.intensity);
	setFloat("Vignette", "Smoothness", screenEffectSettings_.vignette.smoothness);
	setFloat("Vignette", "Roundness", screenEffectSettings_.vignette.roundness);
	Graphics3D::SetPostProcessParameter(postProcessProfile_, "Vignette", "Color", screenEffectSettings_.vignette.color);
	setEnabled("Sharpen", screenEffectSettings_.sharpen.enabled);
	setFloat("Sharpen", "Intensity", screenEffectSettings_.sharpen.intensity);
	setFloat("Sharpen", "Threshold", screenEffectSettings_.sharpen.threshold);

	setEnabled("FXAA", screenEffectSettings_.fxaa.enabled);
	setFloat("FXAA", "EdgeThreshold", screenEffectSettings_.fxaa.edgeThreshold);
	setFloat("FXAA", "MinimumEdgeThreshold", screenEffectSettings_.fxaa.minimumEdgeThreshold);
	setFloat("FXAA", "BlendStrength", screenEffectSettings_.fxaa.blendStrength);
	setEnabled("Pixelation", screenEffectSettings_.pixelationEnabled);
	setFloat("Pixelation", "PixelSize", screenEffectSettings_.pixelSize);
	setEnabled("OrderedDithering", screenEffectSettings_.orderedDithering.enabled);
	setFloat("OrderedDithering", "ColorSteps", screenEffectSettings_.orderedDithering.colorSteps);
	setFloat("OrderedDithering", "Intensity", screenEffectSettings_.orderedDithering.intensity);
	setFloat("OrderedDithering", "Scale", screenEffectSettings_.orderedDithering.scale);
}
#endif
