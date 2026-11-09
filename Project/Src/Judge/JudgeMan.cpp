#include "JudgeMan.h"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/LGFMath.h"

#include "AppMath/AppMath.h"
#include "../Player/Player.h"
#include "LGF/Input.h"

#include "ConversationData.h"
#include "Input/XboxController.h"

#include <cassert>
#include <random>

using namespace LGF;

JudgeMan::JudgeMan() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;
	message_ = std::make_unique<Message>();
}

void JudgeMan::Update(const Player* player, const LGF::DebugCamera3D& camera) {
	message_->Update(camera);
	const auto xboxController = XboxController::Get();
	const bool isTalkTriggered = Key::Space.Trigger() ||
		(xboxController.has_value() &&
			xboxController->Button(GamepadButtonCode::South).Trigger());

	// 審査員がIdle状態のとき
	if (state_ == JudgeManState::JudgeManState_Idle) {
		// プレイヤーの位置を取得
		LGF::Vector3 playerPos = player->GetPosition();

		// プレイヤーと審査員の距離を計算
		float distance = Math::Length(playerPos - position_);

		// プレイヤーが一定距離以内にいる場合、審査員の状態をTalkingに切り替える
		if (distance <= talkDistance_) {
			// 会話可能UIを表示
			message_->SetIsTalkUIVisible(true);

			// SpaceまたはXbox Aボタンで会話開始
			if (isTalkTriggered) {
				// メッセージを表示
				message_->SetIsDisplay(true);
				// 審査員の状態をTalkingに切り替える
				state_ = JudgeManState::JudgeManState_Talking;
				//会話の内容を登録（ランダム）
				std::vector<std::string> group = {
					"demoA",
					"demoB",
					"demoC"
				};
				//ランダムエンジン
				std::random_device seedGenerator;
				std::mt19937 randomEngine(seedGenerator());
				int randomIndex = std::uniform_int_distribution<int>(0, static_cast<int>(group.size()) - 1)(randomEngine);

				message_->RegisterConversationData({ ConversationData::demos.at(group[randomIndex]) });
			}
		}
		else {
			message_->SetIsTalkUIVisible(false);
		}
	}
	// 審査員がTalking状態のとき
	else if (state_ == JudgeManState::JudgeManState_Talking) {
		// 会話を進める処理
		if (isTalkTriggered) {
			if (!message_->Next()) {
				// 会話が終了した場合、審査員の状態をIdleに切り替える
				state_ = JudgeManState::JudgeManState_Idle;
				// メッセージを非表示にする
				message_->SetIsDisplay(false);
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
