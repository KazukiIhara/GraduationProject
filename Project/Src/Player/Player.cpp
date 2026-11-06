#include "Player.h"

#include "cmath"

#include "LGF/Input.h"
#include "LGF/System.h"
#include "imgui.h"

#include "DebugDraw/DebugDraw.h"
#include "Collision/CollisionManager.h"

using namespace LGF;

Player::Player() { Initialize(); }

void Player::Initialize() {
	// 初期位置
	position_ = kInitPosition;

	// コライダー設定
	LGF::Collision::ColliderOptions options{};
	options.tag = "Player";
	options.targetTags = {"Solid", "InteractionSensor"};

	LGF::Collision::AABB colliderAABB{
		// プレイヤーのサイズに合わせて幅1.0, 高さ2.0のボックスにする
	    position_ + kColliderMin, 
		position_ + kColliderMax
    };

	colliderHandle_ = CollisionManager::GetInstance()->AddCollider(colliderAABB, options);
}

void Player::Update(float cameraYaw) {
	/* 中華レンジのインタラクト用センサー判定 */
	auto events = CollisionManager::GetInstance()->GetEvents(colliderHandle_);

	isNearChineseRange_ = false; // 毎フレームリセット

	auto checkSensor = [&](const auto& eventList) {
		for (const auto& collision : eventList) {
			if (collision.tag == "InteractionSensor") {
				isNearChineseRange_ = true;
			}
		}
	};
	checkSensor(events.enter);
	checkSensor(events.stay);

	// 範囲内にいる状態でのボタン入力でモード切り替え
	bool isInteractTriggerd = false;
	if (Key::Enter.Trigger()) {
		isInteractTriggerd = true;
	}

	auto rightJoyCon = JoyCon::Right();
	if (rightJoyCon.has_value() && rightJoyCon->IsConnected()) {
		const LGF::GamepadButton aButton = rightJoyCon->Button(GamepadButtonCode::East);
		if (aButton.Trigger()) {
			isInteractTriggerd = true;
		}
	}

	bool wasCookingMode = isCookingMode_; // 前フレームのモード状態を保持

	if (isNearChineseRange_ && isInteractTriggerd) {
		isCookingMode_ = !isCookingMode_;
	}

	// 料理モードが解除された瞬間の位置変更（ちょっと後ろに配置）
	if (wasCookingMode && !isCookingMode_) {
		position_ = {0.0f, 2.0f, -10.0f}; 
	}

	if (isCookingMode_) {
		position_ = {0.0f, 2.0f, -2.0f}; // プレイヤーの位置を固定

		// 移動処理は行わず、コライダーの位置だけ現在の位置に固定して抜ける
		LGF::Collision::AABB finalAABB{position_ + kColliderMin, position_ + kColliderMax};
		CollisionManager::GetInstance()->SetShape(colliderHandle_, finalAABB);
		return;
	}

	/* 移動処理 */
	LGF::Vector3 moveDir = {0.0f, 0.0f, 0.0f};
	if (ProcessInput(moveDir)) {
		// カメラのYaw回転行列を作成
		LGF::Matrix4x4 cameraRotMat = LGF::Math::MakeRotateYMatrix(cameraYaw);
		// 入力方向をカメラの向き基準のワールド方向へ変換
		LGF::Vector3 velocity = LGF::Math::TransformNormal(moveDir, cameraRotMat);
		// プレイヤー自身の向きもカメラの向きに合わせる
		rotation_.y = cameraYaw;
		// 正規化してスピードを掛ける
		velocity = LGF::Math::Normalize(velocity) * kMoveSpeed * static_cast<float>(System::DeltaTime());
		// 位置を更新
		position_ += velocity;
	}

	/* コライダー更新処理 */
	LGF::Collision::AABB updateAABB{
		position_ + kColliderMin, 
		position_ + kColliderMax
	};
	CollisionManager::GetInstance()->SetShape(colliderHandle_, updateAABB);

	auto applyPushBack = [&](const auto& eventList) {
		// タグを捜査して押し戻しを行う
		for (const auto& collision : eventList) {
			if (collision.tag == "Solid") {
				position_ -= collision.contact.normal * collision.contact.penetrationDepth;
			}
		}
	};

	applyPushBack(events.enter);
	applyPushBack(events.stay);

	LGF::Collision::AABB finalAABB{position_ + kColliderMin, position_ + kColliderMax};
	CollisionManager::GetInstance()->SetShape(colliderHandle_, finalAABB);
}

void Player::Draw() const {
	// モデル描画（デバッグ）
	/*if (model_.IsValid()) {
	    Matrix4x4 worldMatrix = Math::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, Math::MakeQuaternion(rotation_), position_);
	    model_.Draw(worldMatrix);
	}*/

#ifdef _DEBUG
	// コライダーデバッグ描画
	auto stateOpt = CollisionManager::GetInstance()->GetColliderState(colliderHandle_);
	if (stateOpt.has_value()) {
		if (std::holds_alternative<LGF::Collision::AABB>(stateOpt->shape)) {
			DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(stateOpt->shape), {0.0f, 1.0f, 0.0f, 1.0f});
		}
	}
#endif
}

void Player::DrawImGuiDebug() {
#ifdef _DEBUG
	ImGui::Begin("Player");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

	ImGui::Checkbox("IsCookingMode", &isCookingMode_);

	ImGui::End();
#endif
}

bool Player::ProcessInput(LGF::Vector3& outMoveDir) {
	outMoveDir = {0.0f, 0.0f, 0.0f};
	bool isPressed = false;

	/* Joy-Con左スティック操作 */
	auto leftJoyCon = JoyCon::Left();
	if (leftJoyCon.has_value() && leftJoyCon->IsConnected()) {
		Vector2 stick = leftJoyCon->LeftStick();

		// デッドゾーン処理
		if (std::abs(stick.x) < kDeadZone)
			stick.x = 0.0f;
		if (std::abs(stick.y) < kDeadZone)
			stick.y = 0.0f;

		if (std::abs(stick.x) > 0.1f || std::abs(stick.y) > 0.1f) {
			outMoveDir.x = stick.x;
			outMoveDir.z = stick.y;
			return true;
		}
	}

	/* キーボード操作 */
	if (Key::W.Press()) {
		outMoveDir.z += 1.0f;
		isPressed = true;
	}
	if (Key::S.Press()) {
		outMoveDir.z -= 1.0f;
		isPressed = true;
	}
	if (Key::A.Press()) {
		outMoveDir.x -= 1.0f;
		isPressed = true;
	}
	if (Key::D.Press()) {
		outMoveDir.x += 1.0f;
		isPressed = true;
	}

	return isPressed;
}
