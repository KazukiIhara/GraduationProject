#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"
#include "LGF/Graphics/Sprite.h"
#include "Message.h"

class Player;


enum class JudgeManState
{
	JudgeManState_Idle,			// 待機状態
	JudgeManState_Talking,		// 会話中
};

// =========================================================
// 審査員クラス
// =========================================================
class JudgeMan
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	JudgeMan();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Player* player, const LGF::DebugCamera3D& camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const;

	/// <summary>
	/// ImGuiデバッグ描画
	/// </summary>
	void DrawImGuiDebug();

private:
	// =========================================================
	// Internal Methods
	// =========================================================

private:
	// =========================================================
	// Instance
	// =========================================================

	std::unique_ptr<Message> message_;

	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 0.0f, 0.0f, 9.0f };
	const LGF::Vector3 kBaseRotation = { 0.0f, 0.0f, 0.0f, };


	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{ "../Assets/Models/judgeman.gltf" }; // モデル
	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転

	//会話変数
	JudgeManState state_ = JudgeManState::JudgeManState_Idle; // 審査員の状態
	const float talkDistance_ = 8.0f; // プレイヤーとの会話距離
	LGF::Texture textureCanTalkUI_{ "../Assets/Textures/opeA.png" }; // 会話可能時のUIテクスチャ
	LGF::Sprite canTalkUI_{ textureCanTalkUI_ };		// 会話可能時のUI
	bool isCanTalkUIVisible_ = false; // 会話可能UIの表示状態

};

