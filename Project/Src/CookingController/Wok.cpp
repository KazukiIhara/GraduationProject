#include "Wok.h"

#include "imgui.h"

using namespace LGF;

Wok::Wok() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;

	calibration_ = std::make_unique<Calibration>();
}

void Wok::Update() {
	float pi = 3.14159265f;

    // アニメーション未稼働＆上下判定取得したら
    if (!isAnimationPlaying_ &&
        calibration_->GetShakeStrength(
            JoyConType::Left, DeadZoneType::Large)) {

        isAnimationPlaying_ = true;
        animationTime_ = 0.0f;
    }

    // アニメーション稼働中なら
    if (isAnimationPlaying_) {
        // アニメーション時間を加算
        animationTime_ += static_cast<float>(System::DeltaTime());

        // 進行度を0.0～1.0に制限
        float progress = animationTime_ / animationDuration_;
        progress = std::min(progress, 1.0f);

        // 初期位置・初期回転
        position_ = kBasePosition;
        rotation_ = kBaseRotation;

        // 上下移動量
        constexpr float kLiftHeight = 0.5f;

        // 0 → 1 → 0 の上下移動
        position_.y = kBasePosition.y + kLiftHeight * std::sin(
            progress * pi
        );

        // 回転角度（ラジアン）
        constexpr float kTiltAngle = 0.4f;

        // 回転を変化させる
        float angle = kTiltAngle * std::sin(
            progress * 2.0f * pi
        );

        rotation_.x = kBaseRotation.x + angle;

        // アニメーション終了
        if (animationTime_ >= animationDuration_) {
            isAnimationPlaying_ = false;
            position_ = kBasePosition;
            rotation_ = kBaseRotation;
        }
    }
}

void Wok::Draw() const {
	// モデル描画
	if (model_.IsValid()) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f },
			Math::MakeQuaternion(rotation_),
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