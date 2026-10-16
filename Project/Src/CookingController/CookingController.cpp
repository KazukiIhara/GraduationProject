#include "CookingController.h"

CookingController::CookingController() {
	wok_ = std::make_unique<Wok>();
	ladle_ = std::make_unique<Ladle>();
}

void CookingController::Update() {
	// 各調理器具の更新
	wok_->Update();
	ladle_->Update();
}

void CookingController::Draw() const {
	// 各調理器具の描画
	wok_->Draw();
	ladle_->Draw();
}

void CookingController::DrawImGuiDebug() {
	// 各調理器具のデバッグ描画
	wok_->DrawImGuiDebug();
	ladle_->DrawImGuiDebug();
}