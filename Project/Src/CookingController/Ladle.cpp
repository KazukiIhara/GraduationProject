#include "Ladle.h"

#include "algorithm"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/LGFMath.h"

#include "AppMath/AppMath.h"

using namespace LGF;

Ladle::Ladle() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;
}

void Ladle::Update() {
	// ジョイコン入力操作
	JoyConInput();

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

void Ladle::JoyConInput() {
	const std::optional<Gamepad> right = JoyCon::Right();
	if(right) {
		// 加速度センサーの取得
		Vector3 accel = right->Acceleration(GamepadSensorType::Right);
		// 前後の動きに対応する軸に掛け合わせる
		float moveDelta = accel.z * sensitivity_ * 0.05f; // 値が大きくなりすぎないよう補正
		// 振った勢いをオフセットに加算
		currentSwingOffsetZ_ += moveDelta;
		// 動かせる距離に限界を設ける
		currentSwingOffsetZ_ = std::clamp(currentSwingOffsetZ_, kMaxBackwardLimit, kMaxForwardLimit);
	}
}
