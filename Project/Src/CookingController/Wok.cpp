#include "Wok.h"

#include "LGF/Parameter.h"

#include "imgui.h"

#include "Input/XboxController.h"

#include <stdexcept>

#include "AppMath/AppMath.h"

using namespace LGF;

namespace {

	constexpr const char* kWokParameterFilePath = "../Assets/Parameters/Wok.json";
	constexpr float kLiftHeight = 0.5f;
	constexpr float kTiltAngle = 0.4f;
	constexpr float kXboxTriggerThreshold = 0.5f;

	struct WokPhysicsParameters {
		Vector3 outerApexOffset;
		float outerRadius;
		float innerRadius;
		float height;
		float restitution;
		float friction;
	};

	template <class T>
	T ReadParameter(const ParameterDocument& document, const char* path) {
		const auto value = document.Get<T>(path);
		if (!value) {
			throw std::runtime_error(std::string("Invalid wok parameter: ") + path);
		}
		return *value;
	}

	WokPhysicsParameters LoadWokPhysicsParameters() {
		ParameterDocument document;
		const ParameterIoResult result = document.Load(kWokParameterFilePath);
		if (!result) {
			throw std::runtime_error(result.message);
		}

		return WokPhysicsParameters{
			.outerApexOffset = ReadParameter<Vector3>(document, "Shape/OuterApexOffset"),
			.outerRadius = ReadParameter<float>(document, "Shape/OuterRadius"),
			.innerRadius = ReadParameter<float>(document, "Shape/InnerRadius"),
			.height = ReadParameter<float>(document, "Shape/Height"),
			.restitution = ReadParameter<float>(document, "Material/Restitution"),
			.friction = ReadParameter<float>(document, "Material/Friction"),
		};
	}

	const WokPhysicsParameters& GetWokPhysicsParameters() {
		static const WokPhysicsParameters parameters = LoadWokPhysicsParameters();
		return parameters;
	}

}

Wok::Wok(Physics::PhysicsWorld& physicsWorld) :
	physicsWorld_(&physicsWorld) {
	basePosition_ = kBasePosition;
	baseRotation_ = kBaseRotation;
	position_ = basePosition_;
	rotation_ = baseRotation_;

	calibration_ = std::make_unique<Calibration>();

	const WokPhysicsParameters& parameters = GetWokPhysicsParameters();
	physicsHandle_ = physicsWorld_->AddKinematicObject(
		Collision::SphericalShellCap{},
		Physics::KinematicObjectOptions{
			.material = {
				.restitution = parameters.restitution,
				.friction = parameters.friction,
			},
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
	if (canShake_ && !isAnimationPlaying_ &&
		(isJoyConShakeTriggered || isXboxShakeTriggered)) {
		isAnimationPlaying_ = true;
		animationTime_ = 0.0f;
	}

	// 鍋振り許可が降りていなければアニメーションを強制ストップ
	if (!canShake_) {
		isAnimationPlaying_ = false;
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
		position_ = basePosition_;
		rotation_ = baseRotation_;

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
	} else {
		// 通常時は線形補間で追従
		constexpr float kFollowLerpRate = 0.25f;
		position_ = AppMath::Lerp(position_, basePosition_, kFollowLerpRate);
		rotation_ = AppMath::Lerp(rotation_, baseRotation_, kFollowLerpRate);
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

	if (ImGui::Button("Trigger Shake Animation")) {
		TriggerShakeAnimation();
	}
	ImGui::Text("Animation: [%s]", isAnimationPlaying_ ? "PLAYING" : "Idle");
	ImGui::ProgressBar(GetAnimationProgress(), ImVec2(-1, 0), "Progress");

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

void Wok::SetBaseTransform(const LGF::Vector3& basePos, const LGF::Vector3& baseRot, bool snapImmediate) {
	basePosition_ = basePos;
	baseRotation_ = baseRot;
	if (snapImmediate) {
		position_ = basePosition_;
		rotation_ = baseRotation_;
		UpdatePhysicsShape();
	}
}

void Wok::UpdatePhysicsShape() {
	if (physicsWorld_ == nullptr) {
		return;
	}

	const WokPhysicsParameters& parameters = GetWokPhysicsParameters();
	const Matrix4x4 rotationMatrix = Math::MakeRotateMatrix(
		Math::MakeQuaternion(rotation_));
	const Vector3 outerApex = position_ + Math::TransformNormal(
		parameters.outerApexOffset,
		rotationMatrix);
	const Vector3 axis = Math::TransformNormal(
		{ 0.0f, 1.0f, 0.0f },
		rotationMatrix);

	physicsWorld_->SetKinematicObjectShape(
		physicsHandle_,
		Collision::SphericalShellCap{
			.outerApex = outerApex,
			.axis = axis,
			.outerRadius = parameters.outerRadius,
			.innerRadius = parameters.innerRadius,
			.height = parameters.height,
		});
}
