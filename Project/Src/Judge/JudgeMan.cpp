#include "JudgeMan.h"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/LGFMath.h"

#include "AppMath/AppMath.h"
#include "../Player/Player.h"
#include "LGF/Input.h"

#include <cassert>

using namespace LGF;

JudgeMan::JudgeMan() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;
}

void JudgeMan::Update(const Player* player) {
	/* Joy-Con右スティック操作 */
	auto rightJoyCon = JoyCon::Right();
	if (rightJoyCon.has_value() && rightJoyCon->IsConnected()) {
		// 審査員がIdle状態のとき
		if (state_ == JudgeManState::JudgeManState_Idle) {
			// プレイヤーの位置を取得
			LGF::Vector3 playerPos = player->GetPosition();

			// プレイヤーと審査員の距離を計算
			float distance = Math::Length(playerPos - position_);

			// プレイヤーが一定距離以内にいる場合、審査員の状態をTalkingに切り替える
			if (distance <= talkDistance_) {
				// AボタンもしくはSpaceで会話
				if (Key::Space.Press()) {
 					state_ = JudgeManState::JudgeManState_Talking;
				}
			}
		}
	}
}

void JudgeMan::Draw() const {
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

void JudgeMan::DrawImGuiDebug()
{
#ifdef _DEBUG
	ImGui::Begin("JudgeMan");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

	ImGui::End();
#endif
}
