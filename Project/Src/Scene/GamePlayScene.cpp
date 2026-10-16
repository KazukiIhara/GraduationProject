#include "GamePlayScene.h"

#include <imgui.h>

#include "LGF/Input.h"
#include "LGF/System.h"

using namespace LGF;

GamePlayScene::GamePlayScene(const InitData& init) :
	IScene(init),
	camera_(15.0f),
	light_({ 0.5f, -1.0f, 0.35f }, Color::White, 1.5f) {

	camera_.translation = { 0.0f, 10.0f, -25.0f };
	/*camera_.yaw = 0.0f;
	camera_.pitch = 0.9f;*/

	player_ = std::make_unique<Player>();
	cookingController_ = std::make_unique<CookingController>();
}

void GamePlayScene::Update() {
	/* カメラ操作 */
	UpdateCamera();

	/* プレイヤー更新 */
	player_->Update(camera_.yaw);
	// カメラの位置をプレイヤーに追従させる
	camera_.translation = player_->GetEyePosition();

	/* 調理器具マネージャーの更新 */
	cookingController_->Update();

#ifdef _DEBUG
	if (ImGui::Begin("GamePlay Debug")) {
		ImGui::DragFloat3("Camera Position", &camera_.translation.x, 0.05f);
		ImGui::DragFloat("Camera Yaw", &camera_.yaw, 0.01f, -6.28f, 6.28f);
		ImGui::DragFloat("Camera Pitch", &camera_.pitch, 0.01f, -1.57f, 1.57f);
	}
	ImGui::End();

	player_->DrawImGuiDebug();
	cookingController_->DrawImGuiDebug();
#endif
}

void GamePlayScene::Draw() const {
	camera_.SetScene();
	light_.SetScene();

	// グリッドの描画
	grid_.Draw(Math::MakeTranslateMatrix({ 0.0f, 0.0f, 0.0f }));

	// --- 中華レンジモデルの描画 ---
	if (chineseRangeModel_.IsValid()) {
		// ワールド行列の作成（位置やスケール、回転を指定）
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f },                                 // スケール (X, Y, Z)
			Math::MakeRotateAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f), // 回転
			{ 0.0f, 1.0f, 0.0f }                                  // 位置
		);

		// 描画オプション
		ModelDrawOptions options{};
		options.color = Color::White;

		// モデル自身のDraw関数を呼び出す
		chineseRangeModel_.Draw(worldMatrix, options);
	}

	// プレイヤー描画
	player_->Draw();
	// 調理器具マネージャーの描画
	cookingController_->Draw();
}

void GamePlayScene::UpdateCamera() {
	float yawDelta = 0.0f;
	float pitchDelta = 0.0f;

	auto rightJoyCon = JoyCon::Right();
	if (rightJoyCon.has_value() && rightJoyCon->IsConnected()) {
		// 右スティックによる視点操作
		Vector2 rightStick = rightJoyCon->RightStick();

		// デッドゾーン処理
		if (std::abs(rightStick.x) < kDeadZone)
			rightStick.x = 0.0f;
		if (std::abs(rightStick.y) < kDeadZone)
			rightStick.y = 0.0f;

		yawDelta += rightStick.x * kStickSensitivity * static_cast<float>(System::DeltaTime());
		pitchDelta -= rightStick.y * kStickSensitivity * static_cast<float>(System::DeltaTime());
	}

	Vector2 mouseDelta = Mouse::Delta();
	// 左右移動で水平・垂直回転
	yawDelta += mouseDelta.x * kMouseSensitivity * static_cast<float>(System::DeltaTime());
	pitchDelta += mouseDelta.y * kMouseSensitivity * static_cast<float>(System::DeltaTime());

	// 最終的なカメラ角度に反映
	camera_.yaw += yawDelta;
	camera_.pitch += pitchDelta;
}