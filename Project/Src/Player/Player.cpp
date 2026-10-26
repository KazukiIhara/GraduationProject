#include "Player.h"

#include "cmath"

#include "LGF/Input.h"
#include "LGF/System.h"
#include "imgui.h"

#include "DebugDraw/DebugDraw.h"

using namespace LGF;

Player::Player() {}

void Player::Initialize(LGF::Collision::ColliderWorld* colliderWorld) {
	colliderWorld_ = colliderWorld;

	// 初期位置
	position_ = kInitPosition;

	// コライダー設定
	LGF::Collision::AABB initialBox{
		// プレイヤーのサイズに合わせて幅1.0, 高さ2.0のボックスにする
	    position_ + LGF::Vector3{-0.5f, 0.0f, -0.5f},
        position_ + LGF::Vector3{0.5f,  2.0f, 0.5f }
    };

	LGF::Collision::ColliderOptions options{};
	options.layer = 1u; // レイヤー設定

	// コライダーワールドに追加してハンドルを保持
	colliderHandle_ = colliderWorld_->Add(initialBox, options);
}

void Player::Update(float cameraYaw) {
	LGF::Vector3 moveDir = {0.0f, 0.0f, 0.0f};

	/* 移動処理 */
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
	if (colliderWorld_) {
		// 現在位置を基準にAABBの位置を再計算
		LGF::Collision::AABB updateBox{
		    position_ + LGF::Vector3{-1.0f, -2.0f, -1.0f},
            position_ + LGF::Vector3{1.0f,  2.0f,  1.0f }
        };

		colliderWorld_->SetShape(colliderHandle_, updateBox);
	}
}

void Player::Draw() const {
	// モデル描画（デバッグ）
	/*if (model_.IsValid()) {
	    Matrix4x4 worldMatrix = Math::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, Math::MakeQuaternion(rotation_), position_);
	    model_.Draw(worldMatrix);
	}*/

#ifdef _DEBUG
	// コライダー描画
	if (colliderWorld_ && colliderWorld_->GetShape(colliderHandle_)) {
		if (const auto* shape = colliderWorld_->GetShape(colliderHandle_)) {
			if (std::holds_alternative<LGF::Collision::AABB>(*shape)) {
				DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(*shape), {1.0f, 1.0f, 1.0f, 1.0f});
			}
		}
	}
#endif
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
