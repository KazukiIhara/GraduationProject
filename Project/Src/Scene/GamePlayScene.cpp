#include "GamePlayScene.h"

#include <imgui.h>

#include "LGF/Input.h"

using namespace LGF;

GamePlayScene::GamePlayScene(const InitData& init) :
	IScene(init),
	camera_(15.0f),
	light_({ 0.5f, -1.0f, 0.35f }, Color::White, 1.5f) {
	camera_.translation = { 0.0f, 4.8f, -1.9f };
	camera_.yaw = 0.0f;
	camera_.pitch = 0.9f;

	cookingController_ = std::make_unique<CookingController>();
	judgeManager_ = std::make_unique<JudgeManager>();
}

void GamePlayScene::Update() {
	// 調理器具マネージャーの更新
	cookingController_->Update();
	// 審査員マネージャーの更新
	judgeManager_->Update();

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

	// 調理器具マネージャーの描画
	cookingController_->Draw();
	// 審査員マネージャーの描画
	judgeManager_->Draw();

#ifdef _DEBUG
	if (ImGui::Begin("GamePlay Debug")) {
		ImGui::DragFloat3("Camera Position", &camera_.translation.x, 0.05f);
		ImGui::DragFloat("Camera Yaw", &camera_.yaw, 0.01f, -6.28f, 6.28f);
		ImGui::DragFloat("Camera Pitch", &camera_.pitch, 0.01f, -1.57f, 1.57f);
	}
	ImGui::End();

	cookingController_->DrawImGuiDebug();
	judgeManager_->DrawImGuiDebug();
#endif
}
