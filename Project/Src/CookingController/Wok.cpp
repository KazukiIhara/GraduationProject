#include "Wok.h"

#include "imgui.h"

using namespace LGF;

Wok::Wok() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;

	calibration_ = std::make_unique<Calibration>();
}

void Wok::Update() {
	////アニメーション未稼働＆上下判定取得したら
	//if (!isAnimationPlaying_ && calibration_->GetShakeStrength(JoyConType::Left,DeadZoneType::Large)) {
	//	//アニメーション稼働
	//	isAnimationPlaying_ = true;
	//	animationTime_ = 0.0f;
	//}

	////アニメーション稼働中なら
	//if (isAnimationPlaying_) {
	//	//アニメーション時間を加算
	//	animationTime_ += Time::DeltaTime();
	//	//アニメーションの進行度を計算（0.0～1.0）
	//	float progress = animationTime_ / animationDuration_;
	//	//アニメーションの進行度に応じて位置と回転を補間
	//	position_ = Math::Lerp(kBasePosition, kBasePosition + Vector3{ 0.0f, 0.5f, 0.0f }, progress);
	//	rotation_ = Math::Lerp(kBaseRotation, kBaseRotation + Vector3{ 30.0f, 0.0f, 0.0f }, progress);
	//	//アニメーションが終了したら
	//	if (animationTime_ >= animationDuration_) {
	//		isAnimationPlaying_ = false;
	//		position_ = kBasePosition;
	//		rotation_ = kBaseRotation;
	//	}
	//}

}

void Wok::Draw() const {
	// モデル描画
	if (model_.IsValid()) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f },
			Math::MakeRotateAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f),
			position_
		);

		ModelDrawOptions options{};
		options.color = Color::White;

		model_.Draw(worldMatrix, options);
	}
}

void Wok::DrawImGuiDebug() {
#ifdef _DEBUG
	ImGui::Begin("Wok");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

	ImGui::End();
#endif	
}