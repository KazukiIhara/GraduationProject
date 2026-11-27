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
	basePosition_ = kBasePosition; 
	baseRotation_ = kBaseRotation;
	position_ = basePosition_;
	rotation_ = baseRotation_;
}

void Ladle::Update() {
	// コントローラー入力操作（仮実装のため一時的にコメントアウト）
	/*ControllerInput();*/

	float targetZ = basePosition_.z + currentSwingOffsetZ_;

	// 位置・回転の補間
	constexpr float kFollowLerpRate = 0.3f;
	position_ = AppMath::Lerp(position_, {basePosition_.x, basePosition_.y, targetZ}, kFollowLerpRate);
	rotation_ = AppMath::Lerp(rotation_, baseRotation_, kFollowLerpRate);
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

void Ladle::SetBaseTransform(const LGF::Vector3& basePos, const LGF::Vector3& baseRot, bool snapImmediate) {
	basePosition_ = basePos;
	baseRotation_ = baseRot;
	if (snapImmediate) {
		position_ = basePosition_;
		rotation_ = baseRotation_;
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
