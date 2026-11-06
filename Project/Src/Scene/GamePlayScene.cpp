#include "GamePlayScene.h"

#include <imgui.h>

#include "LGF/Input.h"
#include "LGF/System.h"

#include <algorithm>

#include "Collision/CollisionManager.h"

using namespace LGF;

GamePlayScene::GamePlayScene(const InitData& init) :
	IScene(init),
	camera_(15.0f),
	light_({ 0.5f, -1.0f, 0.35f }, Color::White, 1.5f) {

	camera_.translation = { 0.0f, 7.0f, -20.0f };
	camera_.yaw = 0.0f;
	camera_.pitch = 0.3f;

	/* コリジョンマネージャー設定 */
	auto* collisionManager = CollisionManager::GetInstance();
	// 一応クリア
	collisionManager->Clear();
	// タグの登録
	collisionManager->RegisterTag("Player"); // プレイヤー
	collisionManager->RegisterTag("Solid"); // 固体（プレイヤーとの押し戻しを行うオブジェクト）
	collisionManager->RegisterTag("InteractionSensor"); // インタラクト用センサー（プレイヤーの中華レンジ周辺でのモード切替用）

	player_ = std::make_unique<Player>();

	cookingController_ = std::make_unique<CookingController>();

	stageManager_ = std::make_unique<StageManager>();

	judgeManager_ = std::make_unique<JudgeManager>();
}

void GamePlayScene::Update() {
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
	if (Key::O.Trigger()) { // Oキーで有効状態切替
		isCameraFollowEnabled_ = !isCameraFollowEnabled_;
	}
	if (isCameraFollowEnabled_) {
		camera_.translation = player_->GetEyePosition();
	}
	/* 調理器具マネージャーの更新 */
	cookingController_->Update();
	/* ステージオブジェクト管理クラスの更新 */
	stageManager_->Update();
	// 審査員マネージャーの更新
	judgeManager_->Update(player_.get(), camera_);

	/* コリジョンマネージャー更新 */
	CollisionManager::GetInstance()->Step();

#ifdef _DEBUG
	if (ImGui::Begin("GamePlay Debug")) {
		// 現在の設定状態
		ImGui::Text("P : Toggle Mouse Camera"); 
		ImGui::SameLine();
		ImGui::Text("[%s]", isMouseCameraEnabled_ ? "Enabled" : "Disabled");

		ImGui::Text("O : Toggle Camera Follow");
		ImGui::SameLine();
		ImGui::Text("[%s]", isCameraFollowEnabled_ ? "Enabled" : "Disabled");

		ImGui::Separator();

		// カメラ情報
		ImGui::DragFloat3("Camera Position", &camera_.translation.x, 0.05f);
		ImGui::DragFloat("Camera Yaw", &camera_.yaw, 0.01f, -6.28f, 6.28f);
		ImGui::DragFloat("Camera Pitch", &camera_.pitch, 0.01f, -1.57f, 1.57f);
	}
	ImGui::End();

	player_->DrawImGuiDebug();
	cookingController_->DrawImGuiDebug();
	stageManager_->DrawImGuiDebug();

	CollisionManager::GetInstance()->DrawImGuiDebug();
#endif
}

void GamePlayScene::Draw() const {
	skybox_.SetScene();

	camera_.SetScene();
	light_.SetScene();

	// グリッドの描画
	grid_.Draw(Math::MakeTranslateMatrix({ 0.0f, 0.0f, 0.0f }));

	// プレイヤー描画
	player_->Draw();
	// 調理器具マネージャーの描画
	cookingController_->Draw();
	// ステージオブジェクト管理クラスの描画
	stageManager_->Draw();
	// 審査員マネージャーの描画
	judgeManager_->Draw();
}

void GamePlayScene::UpdateCamera() {
	float yawDelta = 0.0f;
	float pitchDelta = 0.0f;

	/* 右スティックによる視点操作 */
	auto rightJoyCon = JoyCon::Right();
	if (rightJoyCon.has_value() && rightJoyCon->IsConnected()) {
		Vector2 rightStick = rightJoyCon->RightStick();

		// デッドゾーン処理
		if (std::abs(rightStick.x) < kDeadZone)
			rightStick.x = 0.0f;
		if (std::abs(rightStick.y) < kDeadZone)
			rightStick.y = 0.0f;

		yawDelta += rightStick.x * kStickSensitivity * static_cast<float>(System::DeltaTime());
		pitchDelta -= rightStick.y * kStickSensitivity * static_cast<float>(System::DeltaTime());
	}

	/* マウスによる視点操作（デバッグ） */
	if (Key::P.Trigger()) { // Pキーで入力有効状態切替
		isMouseCameraEnabled_ = !isMouseCameraEnabled_; 
	}
	if (isMouseCameraEnabled_) {
		Vector2 mouseDelta = Mouse::Delta();
		yawDelta += mouseDelta.x * kMouseSensitivity * static_cast<float>(System::DeltaTime());
		pitchDelta += mouseDelta.y * kMouseSensitivity * static_cast<float>(System::DeltaTime());
	}

	cookingController_->DrawImGuiDebug();
	judgeManager_->DrawImGuiDebug();

	// 最終的なカメラ角度に反映
	camera_.yaw += yawDelta;
	camera_.pitch += pitchDelta;

	// Pitchの回転制限
	camera_.pitch = std::clamp(camera_.pitch, -kPitchLimit, kPitchLimit);
}