#include "Wok.h"

#include "imgui.h"

#include "Input/XboxController.h"

using namespace LGF;

namespace {

	constexpr Vector3 kWokOuterApexOffset = { 0.0f, -0.37f, 0.0f };
	constexpr float kWokOuterRadius = 1.1f;
	constexpr float kWokInnerRadius = 1.0f;
	constexpr float kWokHeight = 0.75f;
	constexpr float kLiftHeight = 0.5f;
	constexpr float kTiltAngle = 0.4f;
	constexpr float kXboxTriggerThreshold = 0.5f;

}

Wok::Wok(Physics::PhysicsWorld& physicsWorld) :
	physicsWorld_(&physicsWorld) {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;

	calibration_ = std::make_unique<Calibration>();

	physicsHandle_ = physicsWorld_->AddKinematicObject(
		Collision::SphericalShellCap{},
		Physics::KinematicObjectOptions{
			.material = { .restitution = 0.05f, .friction = 0.8f },
			.objectHandle = { 10u, 1u },
		});
	UpdatePhysicsShape();
}

Wok::~Wok() {
	if (physicsWorld_ != nullptr) {
		physicsWorld_->RemoveKinematicObject(physicsHandle_);
	}
}

void Wok::Update() {
	const auto xboxController = XboxController::Get();
	const bool isXboxShakeTriggered = xboxController.has_value() &&
		xboxController->LeftTrigger() >= kXboxTriggerThreshold;
	const bool isJoyConShakeTriggered = calibration_->GetShakeStrength(
		JoyConType::Left,
		DeadZoneType::Large);

	// アニメーション未稼働＆上下判定取得したら
	if (!isAnimationPlaying_ &&
		(isJoyConShakeTriggered || isXboxShakeTriggered)) {
		isAnimationPlaying_ = true;
		animationTime_ = 0.0f;
	}

	// アニメーション稼働中なら
	if (isAnimationPlaying_) {
		const float pi = 3.14159265f;

		// アニメーション時間を加算
		animationTime_ += static_cast<float>(System::DeltaTime());

		// 進行度を0.0～1.0に制限
		float progress = animationTime_ / animationDuration_;
		progress = std::min(progress, 1.0f);

		// 初期位置・初期回転
		position_ = kBasePosition;
		rotation_ = kBaseRotation;

		// 0 → 1 → 0 の上下移動
		position_.y = kBasePosition.y + kLiftHeight * std::sin(
			progress * pi
		);

		// 回転を変化させる
		const float angle = kTiltAngle * std::sin(
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

	UpdatePhysicsShape();
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

Vector3 Wok::GetRiceSpawnPosition() const {
	const Matrix4x4 rotationMatrix = Math::MakeRotateMatrix(
		Math::MakeQuaternion(rotation_));
	return position_ + Math::TransformNormal(
		{ 0.0f, 0.65f, 0.0f },
		rotationMatrix);
}

void Wok::UpdatePhysicsShape() {
	if (physicsWorld_ == nullptr) {
		return;
	}

	const Matrix4x4 rotationMatrix = Math::MakeRotateMatrix(
		Math::MakeQuaternion(rotation_));
	const Vector3 outerApex = position_ + Math::TransformNormal(
		kWokOuterApexOffset,
		rotationMatrix);
	const Vector3 axis = Math::TransformNormal(
		{ 0.0f, 1.0f, 0.0f },
		rotationMatrix);

	physicsWorld_->SetKinematicObjectShape(
		physicsHandle_,
		Collision::SphericalShellCap{
			.outerApex = outerApex,
			.axis = axis,
			.outerRadius = kWokOuterRadius,
			.innerRadius = kWokInnerRadius,
			.height = kWokHeight,
		});
}
