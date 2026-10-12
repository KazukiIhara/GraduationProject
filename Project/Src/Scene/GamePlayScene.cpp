#include "GamePlayScene.h"

#include <imgui.h>

#include "LGF/Input.h"

using namespace LGF;

GamePlayScene::GamePlayScene(const InitData& init) :
	IScene(init),
	camera_(15.0f),
	light_({ 0.5f, -1.0f, 0.35f }, Color::White, 1.5f) {
	camera_.pitch = 20.0f * Math::DEG_TO_RAD;
}

void GamePlayScene::Update() {
	// スペースキーが押された瞬間を検知してモード切替
	if (LGF::Key::Space.Trigger()) {
		if (currentMode_ == GameMode::Cooking) {
			currentMode_ = GameMode::Hunting;

			// 狩りモード：後ろの草原へ移動し、後ろ側（または適切な方向）を向く
			camera_.translation = { 0.0f, 1.8f, -2.0f };
			camera_.yaw = 3.14159f; // 180度回転（後ろを向く場合：Math::PI に相当するラジアン）
			camera_.pitch = 0.0f;
		} else {
			currentMode_ = GameMode::Cooking;

			// 料理モード：キッチンの位置に戻り、前（カウンター）を向く
			camera_.translation = { 0.0f, 4.8f, -1.9f };
			camera_.yaw = 0.0f;     // 正面を向く
			camera_.pitch = 0.9f;
		}
	}
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

	// --- 中華鍋モデルの描画 ---
	if (wokModel_.IsValid()) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			{1.0f, 1.0f, 1.0f},
			Math::MakeRotateAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f), 
			{ 0.0f, 2.5f, 0.0f }                                  
		);

		ModelDrawOptions options{};
		options.color = Color::White;

		wokModel_.Draw(worldMatrix, options);
	}

	// --- おたまモデルの描画 --- 
	if (ladlesModel_.IsValid()) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f },
			Math::MakeRotateAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f),
			{ 1.5f, 2.5f, 0.0f }
		);

		ModelDrawOptions options{};
		options.color = Color::White;

		ladlesModel_.Draw(worldMatrix, options);
	}

#ifdef _DEBUG
	// --- ImGui によるデバッグウィンドウ ---
	ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(350.0f, 220.0f), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Camera Debug & Control")) {
		ImGui::Text("Current Mode: %s", (currentMode_ == GameMode::Cooking) ? "Cooking" : "Hunting");
		ImGui::TextDisabled("Press Space to switch mode");
		ImGui::Separator();

		// mutable にした camera_.translation を直接スライダーに渡す
		if (ImGui::DragFloat3("Position", &camera_.translation.x, 0.05f)) {
			
		}

		if (ImGui::DragFloat("Yaw (Rad)", &camera_.yaw, 0.01f, -6.28f, 6.28f)) {
		}

		if (ImGui::DragFloat("Pitch (Rad)", &camera_.pitch, 0.01f, -1.57f, 1.57f)) {
		}

		ImGui::Separator();
		if (ImGui::Button("Reset Camera")) {
			camera_.translation = { 0.0f, 1.6f, 0.0f };
			camera_.yaw = 0.0f;
			camera_.pitch = 0.0f;
		}
	}
	ImGui::End();
#endif
}
