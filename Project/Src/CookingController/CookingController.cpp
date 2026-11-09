#include "CookingController.h"

#include <cmath>
#include "LGF/System.h"

CookingController::CookingController() {
	wok_ = std::make_unique<Wok>();
	ladle_ = std::make_unique<Ladle>();
	cookingSystem_ = std::make_unique<CookingSystem>();

	prevWokShaking_ = false;
	prevLadleOffsetZ_ = 0.0f;
}

void CookingController::Update(bool isCookingMode) {
	// 各調理器具の更新
	wok_->Update();
	ladle_->Update();

	float dt = static_cast<float>(LGF::System::DeltaTime());

	// 1. 鍋振りのトリガー検知
	// （※将来、WokやPlayerに向き・強さ取得関数が実装されたらここから渡す）
	bool currentShaking = wok_->IsAnimationPlaying();
	if (!prevWokShaking_ && currentShaking) {
		// 現時点の仮値：縦あおり（Vertical）、中（Medium）
		// TODO: wok_->GetShakeDirection(), wok_->GetShakeStrength() 等の実装時に差し替え
		WokShakeDirection direction = WokShakeDirection::Vertical;
		ShakeStrengthLevel strength = ShakeStrengthLevel::Medium;
		cookingSystem_->OnWokShake(direction, strength);
	}
	prevWokShaking_ = currentShaking;

	// 2. おたまのスイング量から炒め強度を反映
	float currentOffsetZ = ladle_->GetCurrentSwingOffsetZ();
	float deltaOffsetZ = currentOffsetZ - prevLadleOffsetZ_;
	cookingSystem_->ProcessLadleSwing(deltaOffsetZ, dt);
	prevLadleOffsetZ_ = currentOffsetZ;

	// 3. 調理システムの毎フレーム更新
	// 調理場ステート（isCookingMode: プレイヤーがコンロ前にいるか）により自動で加熱/冷却を制御
	cookingSystem_->Update(
		dt,
		isCookingMode,
		currentShaking,
		std::abs(ladle_->GetSwingNormalized())
	);
}

void CookingController::Draw() const {
	// 各調理器具の描画
	wok_->Draw();
	ladle_->Draw();
}

void CookingController::DrawImGuiDebug() {
	// 各調理器具および調理システムのデバッグ描画
	cookingSystem_->DrawImGuiDebug();
	wok_->DrawImGuiDebug();
	ladle_->DrawImGuiDebug();
}