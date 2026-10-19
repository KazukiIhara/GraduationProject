#include "Message.h"

#include <imgui.h>

#include "LGF/LGFMath.h"
#include "AppMath/AppMath.h"

using namespace LGF;

auto easeOutBack = [](float t) {
	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;

	return 1.0f + c3 * std::pow(t - 1.0f, 3.0f)
		+ c1 * std::pow(t - 1.0f, 2.0f);
	};


Message::Message() {
	messageMaterial_.baseColorTexture = Texture("../Assets/Textures/MessageTexture.png");
	messageMaterial_.uvScale = { -1.0f, 1.0f };

	position_ = kBasePosition;
	rotation_ = kBaseRotation;
	scale_ = kBaseScale;
}

void Message::Update(const LGF::DebugCamera3D& camera) {
	// ビルボード
	Updatebillboard(camera);
	// トランスフォームの更新
	UpdateTransform();

}

void Message::Draw() const {
	message_.Draw(
		Math::MakeAffineMatrix(scale_,
			Math::MakeQuaternion(rotation_), 
			position_
		),
		messageMaterial_
	);
}

void Message::DrawImGuiDebug() {
}

void Message::UpdateTransform() {
	// アニメーションタイマーの更新
	animationTimer_ += static_cast<float>(System::DeltaTime());
	if (animationTimer_ >= animationDuration_) {
		animationTimer_ = 0.0f;
		isHalfway_ = !isHalfway_;
	}


	if (!isHalfway_) {
		// 座標
		position_.y = AppMath::Lerp(kBasePosition.y-positionAmplitude_, kBasePosition.y + positionAmplitude_, easeOutBack(animationTimer_ / animationDuration_));

		// スケール
		scale_.x = AppMath::Lerp(kBaseScale.x-scaleAmplitude_, kBaseScale.x + scaleAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
		scale_.y = AppMath::Lerp(kBaseScale.y-scaleAmplitude_, kBaseScale.y + scaleAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
		scale_.z = AppMath::Lerp(kBaseScale.z-scaleAmplitude_, kBaseScale.z + scaleAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
	}
	else {
		// 座標
		position_.y = AppMath::Lerp(kBasePosition.y + positionAmplitude_, kBasePosition.y - positionAmplitude_, easeOutBack(animationTimer_ / animationDuration_));

		// スケール
		scale_.x = AppMath::Lerp(kBaseScale.x + scaleAmplitude_, kBaseScale.x - scaleAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
		scale_.y = AppMath::Lerp(kBaseScale.y + scaleAmplitude_, kBaseScale.y - scaleAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
		scale_.z = AppMath::Lerp(kBaseScale.z + scaleAmplitude_, kBaseScale.z - scaleAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
	}
}

void Message::Updatebillboard(const LGF::DebugCamera3D& camera) {
	// カメラの位置とメッセージの位置から、カメラから見た方向を計算
	LGF::Vector3 direction = camera.translation - position_;
	// 方向ベクトルを正規化
	direction = LGF::Math::Normalize(direction);
	// メッセージの回転を計算（Y軸回転のみ）
	rotation_.y = std::atan2(direction.x, direction.z);
}