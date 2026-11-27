#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"
#include "LGF/Graphics/Sprite.h"
#include "Message.h"
#include "CookingController/CookingParameters.h"

class Player;
class CookingSystem;


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
	void Update(const Player* player, const LGF::DebugCamera3D& camera, CookingSystem* cookingSystem = nullptr);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const;

	/// <summary>
	/// ImGuiデバッグ描画
	/// </summary>
	void DrawImGuiDebug();

	/// <summary>
	/// 最後に審査した評価結果を取得
	/// </summary>
	const CookingParameters::JudgeEvaluation& GetLatestEvaluation() const { return latestEvaluation_; }
	bool HasEvaluation() const { return hasEvaluation_; }

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


	LGF::ModelAnimationPlayOptions opt;
	LGF::Model model_{ "../Assets/Models/judgeman/normal/judgeman.gltf" }; // モデル
	LGF::ModelAnimator modelAnimator_{ model_ };

	LGF::ModelAnimationPlayOptions optEating;
	LGF::Model modelEating_{ "../Assets/Models/judgeman/eating/judgeman_Eating.gltf" }; // 食べるモデル
	LGF::ModelAnimator modelEatingAnimator_{ modelEating_ };

	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転

	//会話変数
	JudgeManState state_ = JudgeManState::JudgeManState_Idle; // 審査員の状態
	const float talkDistance_ = 8.0f; // プレイヤーとの会話距離

	// 最新の評価結果
	CookingParameters::JudgeEvaluation latestEvaluation_{};
	bool hasEvaluation_ = false;
};

