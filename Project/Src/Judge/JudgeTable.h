#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Math/Vector3.h"

// =========================================================
// 審査テーブルクラス
// =========================================================
class JudgeTable
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	JudgeTable();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

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
	// Constants
	// =========================================================

	const LGF::Vector3 kBasePosition = { 0.0f, 0.0f, 6.0f };
	const LGF::Vector3 kBaseRotation = { 0.0f, 0.0f, 0.0f, };


	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Model model_{ "../Assets/Models/table.gltf" }; // モデル
	LGF::Vector3 position_{}; // 現在の位置
	LGF::Vector3 rotation_{}; // 現在の回転

};

