#pragma once

#include "GameApp.h"

#include "LGF/Graphics/Font3D.h"
#include "LGF/Math/Vector3.h"

/// <summary>
/// 3D空間のメッセージテキスト管理クラス
/// </summary>
class Message
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	Message();

	void Update(const LGF::DebugCamera3D& camera);

	void Draw() const;

	void DrawImGuiDebug();

	// 会話データの登録
	void RegisterConversationData(const std::vector<std::string_view>& conversationData);
	// 会話次へ
	bool Next();

	void SetIsTalkUIVisible(bool isVisible) { isTalkUIVisible_ = isVisible; }

	void SetIsDisplay(bool isDisplay) { isDisplay_ = isDisplay; }

private:
	// =========================================================
	// Private Methods
	// =========================================================

	void UpdateTransform(const LGF::DebugCamera3D& camera);

	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 8.0f, 7.0f, 9.0f };

	// =========================================================
	// Member Variables
	// =========================================================

	//ファイルのパス
	const std::string texturePath_ = "../Assets/Textures/";
	const std::string fontPath_ = "../Assets/Fonts/";
	
	//ひとつ分のメッセージ
	LGF::Font3D message_{ fontPath_ + "OnionGeki/OnionGeki.ttf", 85u };
	LGF::Font3DDrawOptions messageDrawOptions_;
	bool isDisplay_ = false;
	std::vector<std::string_view> conversationData_;
	size_t currentConversationIndex_ = 0;
	LGF::Vector3 messagePosition_;

	// メッセージアニメーションのための変数
	bool isHalfway_ = false;
	float animationTimer_ = 0.0f;
	const float animationDuration_ = 0.2f; // アニメーションの総時間

	// アニメーションの振れ幅（座標とスケール）
	float positionAmplitude_ = 0.1f;
	float scaleAmplitude_ = 0.08f;

	// 会話可能UIのための変数
	LGF::Texture textureTalkUIAvailable_{ "../Assets/Textures/opeA.png" }; // 会話可能時のUIテクスチャ
	LGF::Sprite canTalkUI_{ textureTalkUIAvailable_ };		// 会話可能時のUI
	bool isTalkUIVisible_ = false; // 会話可能UIの表示状態

};