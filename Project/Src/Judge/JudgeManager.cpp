#include "JudgeManager.h"

JudgeManager::JudgeManager() {
	man_ = std::make_unique<JudgeMan>();
	table_ = std::make_unique<JudgeTable>();
}

void JudgeManager::Update() {
	// 各インスタンスの更新
	man_->Update();
	table_->Update();
}

void JudgeManager::Draw() const {
	// 各インスタンスの描画
	man_->Draw();
	table_->Draw();
}

void JudgeManager::DrawImGuiDebug() {
	// 各インスタンスのデバッグ描画
	man_->DrawImGuiDebug();
	table_->DrawImGuiDebug();
}