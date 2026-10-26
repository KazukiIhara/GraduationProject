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
	message_ = std::make_unique<Message>();
}

void JudgeMan::Update(const Player* player, const LGF::DebugCamera3D& camera) {
	message_->Update(camera);

	isCanTalkUIVisible_ = false; // 会話可能UIの表示状態をリセット

	// 審査員がIdle状態のとき
	if (state_ == JudgeManState::JudgeManState_Idle) {
		// プレイヤーの位置を取得
		LGF::Vector3 playerPos = player->GetPosition();

		// プレイヤーと審査員の距離を計算
		float distance = Math::Length(playerPos - position_);

		// プレイヤーが一定距離以内にいる場合、審査員の状態をTalkingに切り替える
		if (distance <= talkDistance_) {
			// 会話可能UIを表示
			isCanTalkUIVisible_ = true;


			// UIを中央に配置
			canTalkUI_.leftTop = { 640.0f - canTalkUI_.size.x / 2.0f,360.0f - canTalkUI_.size.y / 2.0f };

			// AボタンもしくはSpaceで会話
			if (Key::Space.Trigger()) {
				// メッセージを表示
				message_->SetIsDisplay(true);
				// 審査員の状態をTalkingに切り替える
				state_ = JudgeManState::JudgeManState_Talking;
			}
		}
	}
	// 審査員がTalking状態のとき
	else if (state_ == JudgeManState::JudgeManState_Talking) {
		// ここでは仮に会話が終了したらIdle状態に戻す処理を追加
		if (Key::Space.Trigger()) {
			// メッセージを表示
			message_->SetIsDisplay(false);
			// 審査員の状態をIdleに切り替える
			state_ = JudgeManState::JudgeManState_Idle;
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
	// 会話可能UIの描画
	if (isCanTalkUIVisible_) {
		// 仮に矩形を描画する関数を呼び出す（実際には適切な描画関数を使用してください）
		canTalkUI_.Draw(Color::White); // 黄色で描画
	}
	// メッセージの描画
	message_->Draw();
}

void JudgeMan::DrawImGuiDebug()
{
#ifdef _DEBUG
	ImGui::Begin("JudgeMan");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

	ImGui::End();
#endif
	message_->DrawImGuiDebug();
}
