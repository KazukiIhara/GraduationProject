#include "Player.h"

#include "cmath"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/System.h"

using namespace LGF;

Player::Player() { 
	position_ = kInitPosition; 
}

void Player::Update(float cameraYaw) { 
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
}

void Player::Draw() const {
	// モデル描画
	/*if (model_.IsValid()) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, Math::MakeQuaternion(rotation_), position_);
		model_.Draw(worldMatrix);
	}*/
}

void Player::DrawImGuiDebug() {
#ifdef _DEBUG
	ImGui::Begin("Player");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

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
		if (std::abs(stick.x) < kDeadZone) stick.x = 0.0f;
		if (std::abs(stick.y) < kDeadZone) stick.y = 0.0f;

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
