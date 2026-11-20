#include "JudgeMan.h"

#include "imgui.h"
#include "LGF/Input.h"
#include "LGF/LGFMath.h"
#include "LGF/System.h"

#include "AppMath/AppMath.h"
#include "../Player/Player.h"
#include "CookingController/CookingSystem.h"
#include "ConversationData.h"
#include "Input/XboxController.h"

#include <cassert>
#include <random>

using namespace LGF;

JudgeMan::JudgeMan() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;
	message_ = std::make_unique<Message>();

	//アニメーション初期化
	modelAnimator_.Play("Idle", opt);

}

void JudgeMan::Update(const Player* player, const LGF::DebugCamera3D& camera, CookingSystem* cookingSystem) {
	message_->Update(camera);
	const auto xboxController = XboxController::Get();
	const bool isTalkTriggered = Key::C.Trigger() ||
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
				// アニメーションを切り替える
				modelAnimator_.Play("Conversation", opt);

				// 調理システムから炒飯の評価を取得
				std::string selectedGroup = "demoB";
				if (cookingSystem && cookingSystem->GetParameters().riceAmount > 0.0f) {
					latestEvaluation_ = cookingSystem->GetCurrentEvaluation();
					hasEvaluation_ = true;

					// ランクに応じたリアクション
					switch (latestEvaluation_.grade) {
					case CookingParameters::JudgeGrade::Masterpiece: // Sランク（絶品・黄金炒飯）
						selectedGroup = "demoA";
						break;
					case CookingParameters::JudgeGrade::Delicious:   // Aランク（美味）
						selectedGroup = "demoB";
						break;
					case CookingParameters::JudgeGrade::Acceptable:  // Bランク（合格）
					case CookingParameters::JudgeGrade::Unacceptable:// Cランク（不合格・焦げ）
					default:
						selectedGroup = "demoC";
						break;
					}
				} else {
					hasEvaluation_ = false;
					// 米が入っていない場合、デフォルト会話
					std::vector<std::string> group = { "demoA", "demoB", "demoC" };
					std::random_device seedGenerator;
					std::mt19937 randomEngine(seedGenerator());
					int randomIndex = std::uniform_int_distribution<int>(0, static_cast<int>(group.size()) - 1)(randomEngine);
					selectedGroup = group[randomIndex];
				}

				if (ConversationData::demos.count(selectedGroup)) {
					message_->RegisterConversationData({ ConversationData::demos.at(selectedGroup) });
				}
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
				// アニメーションを切り替える
				modelAnimator_.Play("Idle", opt);
				// メッセージを非表示にする
				message_->SetIsDisplay(false);
			}
		}
	}

	//アニメーション更新
	modelAnimator_.Update((float)System::DeltaTime());

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

		model_.Draw(worldMatrix,modelAnimator_, options);
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

	ImGui::Separator();
	if (hasEvaluation_) {
		const char* gradeStr = "Unknown";
		ImVec4 gradeColor(1, 1, 1, 1);
		switch (latestEvaluation_.grade) {
		case CookingParameters::JudgeGrade::Masterpiece:
			gradeStr = "S: Masterpiece (Ultimate)";
			gradeColor = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);
			break;
		case CookingParameters::JudgeGrade::Delicious:
			gradeStr = "A: Delicious (Splendid)";
			gradeColor = ImVec4(0.2f, 1.0f, 0.4f, 1.0f);
			break;
		case CookingParameters::JudgeGrade::Acceptable:
			gradeStr = "B: Acceptable (Passing)";
			gradeColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
			break;
		case CookingParameters::JudgeGrade::Unacceptable:
			gradeStr = "C: Unacceptable (Failed)";
			gradeColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
			break;
		}
		ImGui::TextColored(gradeColor, "Latest Dish Rank: %s", gradeStr);
		ImGui::Text("Total Score: %d / 100", latestEvaluation_.totalScore);
		ImGui::TextWrapped("Comment: %s", latestEvaluation_.comment.c_str());
	} else {
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No dish presented yet.");
	}

	ImGui::End();
#endif
	message_->DrawImGuiDebug();
}
