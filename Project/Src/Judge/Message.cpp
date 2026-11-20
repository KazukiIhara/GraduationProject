#include "Message.h"

#include <imgui.h>
#include <cassert>

#include "LGF/LGFMath.h"
#include "AppMath/AppMath.h"

using namespace LGF;

auto easeOutBack = [](float t) {
	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;

	return 1.0f + c3 * std::pow(t - 1.0f, 3.0f)
		+ c1 * std::pow(t - 1.0f, 2.0f);
	};

Message::Message() {
	//メッセージ描画オプションの初期化
	messageDrawOptions_ = {
		.color = Color::Red,
		.anchor = { 0.5f, 0.5f },
		.worldUnitsPerPixel = 0.01f,
		.mode = Font3DMode::Billboard,
		.rotation = 0.0f
	};

	//メッセージボードのトランスフォーム初期化
	messagePosition_ = kBasePosition;

	// 会話可能UIは中央で初期化
	canTalkUI_.leftTop = { 640.0f - canTalkUI_.size.x / 2.0f,360.0f - canTalkUI_.size.y / 2.0f };
}

void Message::Update(const LGF::DebugCamera3D& camera) {
	// 会話可能UIの表示状態をリセット
	isTalkUIVisible_ = false;

	// トランスフォームの更新
	UpdateTransform(camera);

}

void Message::Draw() const {
	//メッセージの描画
	if (isDisplay_) {
		message_.Draw(
			conversationData_[currentConversationIndex_],
			messagePosition_,
			messageDrawOptions_
		);
	}

	//メッセージ背景の描画
	if (isDisplay_) {
		Vector2 measure = message_.Measure(conversationData_[currentConversationIndex_]);
		Vector2 size = measure * messageDrawOptions_.worldUnitsPerPixel;
	}

	// 会話可能UIの描画
	if (isTalkUIVisible_) {
		// 仮に矩形を描画する関数を呼び出す（実際には適切な描画関数を使用してください）
		canTalkUI_.Draw(Color::White); // 通常色で描画
	}
}

void Message::DrawImGuiDebug() {
}

void Message::RegisterConversationData(const std::vector<std::string_view>& conversationData)
{
	conversationData_ = conversationData;

	//インデックスをリセット
	currentConversationIndex_ = 0;
}

bool Message::Next()
{
	//インクリメント
	++currentConversationIndex_;

	//次がない場合
	if (currentConversationIndex_ >= conversationData_.size()) {
		// インデックスをリセット
		currentConversationIndex_ = 0;
		// メッセージを非表示にする
		isDisplay_ = false;
		// データを破棄
		conversationData_.clear();

		return false;
	}

	return true;

}

void Message::UpdateTransform(const LGF::DebugCamera3D& camera) {
	camera;
	// アニメーションタイマーの更新
	animationTimer_ += static_cast<float>(System::DeltaTime());
	if (animationTimer_ >= animationDuration_) {
		animationTimer_ = 0.0f;
		isHalfway_ = !isHalfway_;
	}


	if (!isHalfway_) {
		// 座標
		messagePosition_.y = AppMath::Lerp(kBasePosition.y - positionAmplitude_, kBasePosition.y + positionAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
	}
	else {
		// 座標
		messagePosition_.y = AppMath::Lerp(kBasePosition.y + positionAmplitude_, kBasePosition.y - positionAmplitude_, easeOutBack(animationTimer_ / animationDuration_));
	}

}
