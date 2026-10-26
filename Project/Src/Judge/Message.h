#pragma once

#include "GameApp.h"

#include "LGF/Graphics/Plane3D.h"
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

	void SetIsDisplay(bool isDisplay) { isDisplay_ = isDisplay; }

private:
	// =========================================================
	// Private Methods
	// =========================================================

	void UpdateTransform();

	void Updatebillboard(const LGF::DebugCamera3D& camera);


	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 5.0f, 5.0f, 9.0f };
	const LGF::Vector3 kBaseRotation = { 0.0f, 0.0f, 0.0f, };
	const LGF::Vector3 kBaseScale = { 5.0f, 3.0f, 1.0f };

	// =========================================================
	// Member Variables
	// =========================================================

	//ひとつ分のメッセージ
	LGF::Plane3D message_;
	LGF::Material3D messageMaterial_;
	bool isDisplay_ = false;

	// メッセージアニメーションのための変数
	bool isHalfway_ = false;
	float animationTimer_ = 0.0f;
	const float animationDuration_ = 0.2f; // アニメーションの総時間

	// アニメーションの振れ幅（座標とスケール）
	float positionAmplitude_ = 0.1f;
	float scaleAmplitude_ = 0.08f;

	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転
	LGF::Vector3 scale_{}; // 現在のスケール	
};