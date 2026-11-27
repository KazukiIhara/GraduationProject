#include "CookingController.h"

#include <cmath>
#include "LGF/System.h"

CookingController::CookingController(LGF::Physics::PhysicsWorld& physicsWorld) {
	wok_ = std::make_unique<Wok>(physicsWorld);
	ladle_ = std::make_unique<Ladle>();
	cookingSystem_ = std::make_unique<CookingSystem>();
	cookingSystem_->BindWok(wok_.get());

	prevWokShaking_ = false;
	prevLadleOffsetZ_ = 0.0f;
}

void CookingController::Update(
    bool isCookingMode, bool justEnteredCookingMode, const LGF::Vector3& leftHandPos, const LGF::Vector3& rightHandPos,const LGF::Vector3& playerRot) {
	// 料理モード中のみ鍋振り許可（移動中は不可）
	wok_->SetCanShake(isCookingMode);

	// 調理中は固定位置
	if (justEnteredCookingMode) {
		wok_->SetBaseTransform({0.0f, 2.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, true);
		ladle_->SetBaseTransform({0.15f, 3.0f, -0.15f}, {-1.35f, 0.0f, -0.25f}, true);
	// 調理中でなければ両手位置に追従
	} else if (!isCookingMode) {
		wok_->SetBaseTransform(leftHandPos, {0.0f, playerRot.y, 0.0f}, false);
		ladle_->SetBaseTransform(rightHandPos, {-1.35f, playerRot.y, -0.25f}, false);
	}

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
