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

private:
	// =========================================================
	// Private Methods
	// =========================================================

	void Updatebillboard(const LGF::DebugCamera3D& camera);


	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 5.0f, 5.0f, 9.0f };
	const LGF::Vector3 kBaseRotation = { 0.0f, 0.0f, 0.0f, };

	// =========================================================
	// Member Variables
	// =========================================================

	//ひとつ分のメッセージ
	LGF::Plane3D message_;
	Material3D messageMaterial_;

	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転
};

