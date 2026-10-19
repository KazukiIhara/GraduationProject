#include "JudgeManager.h"

JudgeManager::JudgeManager() {
	man_ = std::make_unique<JudgeMan>();
	table_ = std::make_unique<JudgeTable>();
	message_ = std::make_unique<Message>();
}

void JudgeManager::Update(const LGF::DebugCamera3D& camera) {
	// 各インスタンスの更新
	man_->Update();
	table_->Update();
	message_->Update(camera);
}

void JudgeManager::Draw() const {
	// 各インスタンスの描画
	man_->Draw();
	table_->Draw();
	message_->Draw();
}

void JudgeManager::DrawImGuiDebug() {
	// 各インスタンスのデバッグ描画
	man_->DrawImGuiDebug();
	table_->DrawImGuiDebug();
	message_->DrawImGuiDebug();
}