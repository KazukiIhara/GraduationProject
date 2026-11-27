#include "Ladle.h"

#include "algorithm"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/LGFMath.h"
#include "LGF/System.h"

#include "AppMath/AppMath.h"
#include "DebugDraw/DebugDraw.h"
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

Ladle::~Ladle() {
}

void Ladle::Update() {
	const float dt = static_cast<float>(System::DeltaTime());

	// コントローラー入力操作（仮実装のため一時的にコメントアウト）
	/*ControllerInput();*/

	// 攻撃入力とモーション更新
	if (canAttack_) {
		AttackInput();
	}
	UpdateAttack(dt);

	// プレイヤーの向きから前方向ベクトルを算出
	const Matrix4x4 yawMat = Math::MakeRotateYMatrix(baseRotation_.y);
	const Vector3 forwardDir = Math::Normalize(Math::TransformNormal({0.0f, 0.0f, 1.0f}, yawMat));

	Vector3 targetPos = basePosition_;
	Vector3 targetRot = baseRotation_;

	// モーション中のオフセット加算
	if (isAttacking_) {
		const float progress = std::clamp(attackTimer_ / kAttackDuration, 0.0f, 1.0f);
		const float curve = std::sinf(progress * Math::PI);

		// プレイヤー前方向へ突き出す目標位置・回転
		targetPos += forwardDir * (kThrustDistance * curve);
		targetRot += kThrustRotOffset * curve;

		// 横振り
		if (currentAttackType_ == AttackType::Horizontal) {
			const float dir = std::sinf(progress * Math::PI * 2.0f);
			const Vector3 rotatedMoveOffset = Math::TransformNormal(kHorizontalMoveOffset * dir, yawMat);
			targetPos += rotatedMoveOffset;
			targetRot += kHorizontalRotOffset * dir;
		// 縦振り
		} else {
			float verticalFactor = 0.0f;
			if (progress < 0.25f) {
				// 前半は上へ振り上げる
				const float windupT = progress / 0.25f;
				const float easeUp = std::sinf(windupT * (Math::PI * 0.5f));
				verticalFactor = -0.5f * easeUp;
			} else {
				// 後半は上から下へ振り下ろす
				const float slashT = (progress - 0.25f) / 0.75f;
				const float easeDown = slashT * slashT;
				verticalFactor = -0.5f + 1.5f * easeDown;
			}
			const Vector3 rotatedMoveOffset = Math::TransformNormal(kVerticalMoveOffset * verticalFactor, yawMat);
			targetPos += rotatedMoveOffset;
			targetRot += kVerticalRotOffset * verticalFactor;
		}
	}

	// 突き出し時・元の位置へ戻る時のどちらも線形補間で移動
	const float lerpRate = isAttacking_ ? kAttackMotionLerpRate : 0.2f;
	position_ = AppMath::Lerp(position_, targetPos, lerpRate);
	rotation_ = AppMath::Lerp(rotation_, targetRot, lerpRate);
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

#ifdef _DEBUG
	const LGF::Collision::Sphere tipSphere{
		.center = GetTipWorldPosition(), 
		.radius = kTipSphereRadius
	};
	DebugDraw::DrawSphere(tipSphere.center, tipSphere.radius, {0.0f, 1.0f, 0.0f, 1.0f});
#endif
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

void Ladle::AttackInput() { 
	bool isAttackTriggered = false;

	// 左クリック
	if (Mouse::Left.Trigger()) {
		isAttackTriggered = true;
	}

	// 右Joy-ConのRボタン
	auto rightJoyCon = JoyCon::Right();
	if (rightJoyCon.has_value() && rightJoyCon->IsConnected()) {
		if (rightJoyCon->Button(GamepadButtonCode::RightShoulder).Trigger()) {
			isAttackTriggered = true;
		}
	}

	if (isAttackTriggered && !isAttacking_ && attackCooldownTimer_ <= 0.0f) {
		isAttacking_ = true;
		attackTimer_ = 0.0f;
		attackCooldownTimer_ = kAttackInterval;
		idleTimeSinceLastAttack_ = 0.0f;

		currentAttackType_ = nextAttackType_;
		// 横と縦を交互に切り替え
		nextAttackType_ = (currentAttackType_ == AttackType::Horizontal) ? 
			AttackType::Vertical : AttackType::Horizontal;
	}
}

void Ladle::UpdateAttack(float dt) {
	if (attackCooldownTimer_ > 0.0f) {
		attackCooldownTimer_ = std::max(0.0f, attackCooldownTimer_ - dt);
	}

	//攻撃時処理
	if (isAttacking_) {
		attackTimer_ += dt;
		if (attackTimer_ >= kAttackDuration) {
			isAttacking_ = false;
			attackTimer_ = 0.0f;
			idleTimeSinceLastAttack_ = 0.0f;
		}
	// 非攻撃時処理
	} else {
		idleTimeSinceLastAttack_ += dt;
		// 一定時間攻撃しなければ、横振りから開始にリセット
		if (idleTimeSinceLastAttack_ >= kComboResetTime) {
			nextAttackType_ = AttackType::Horizontal;
		}
	}
}

LGF::Vector3 Ladle::GetTipWorldPosition() const { 
	const Matrix4x4 rotMat = Math::MakeRotateMatrix(Math::MakeQuaternion(rotation_)); 
	return position_ + Math::TransformNormal(kTipOffset, rotMat);
}
