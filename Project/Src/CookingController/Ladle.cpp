#include "Ladle.h"

#include "algorithm"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/LGFMath.h"

#include "AppMath/AppMath.h"
#include "Input/XboxController.h"

using namespace LGF;

namespace {

	constexpr float kXboxTriggerThreshold = 0.5f;
	constexpr float kXboxAcceleration = 1.0f;

}

Ladle::Ladle() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;
}

void Ladle::Update() {
	// コントローラー入力操作
	ControllerInput();

	float targetZ = kBasePosition.z + currentSwingOffsetZ_;

	// 位置の補間
	position_.x = AppMath::Lerp(position_.x, kBasePosition.x, 0.2f);
	position_.y = AppMath::Lerp(position_.y, kBasePosition.y, 0.2f);
	position_.z = AppMath::Lerp(position_.z, targetZ, 0.2f);
}

void Ladle::Draw() const {
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

void Ladle::DrawImGuiDebug()
{
#ifdef _DEBUG
	ImGui::Begin("Ladle");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

	ImGui::End();
#endif
}

void Ladle::ControllerInput() {
	float accelerationZ = 0.0f;
	const std::optional<Gamepad> right = JoyCon::Right();
	if(right) {
		// 加速度センサーの取得
		accelerationZ += right->Acceleration(GamepadSensorType::Right).z;
	}

	const auto xboxController = XboxController::Get();
	if (xboxController.has_value() &&
		xboxController->RightTrigger() >= kXboxTriggerThreshold) {
		// 右トリガーが閾値を超えたら一定の加速度入力として扱う
		accelerationZ += kXboxAcceleration;
	}

	// 前後の動きに対応する軸に掛け合わせる
	const float moveDelta = accelerationZ * sensitivity_ * 0.05f;
	// 振った勢いをオフセットに加算
	currentSwingOffsetZ_ += moveDelta;
	// 動かせる距離に限界を設ける
	currentSwingOffsetZ_ = std::clamp(
		currentSwingOffsetZ_,
		kMaxBackwardLimit,
		kMaxForwardLimit);
}
