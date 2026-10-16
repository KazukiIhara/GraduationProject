#pragma once

#include "JudgeMan.h"
#include "JudgeTable.h"

// =========================================================
// 審査員周り管理クラス
// =========================================================
class JudgeManager
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	JudgeManager();

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
	// Member Variables
	// =========================================================

	// インスタンス
	std::unique_ptr<JudgeMan> man_;
	std::unique_ptr<JudgeTable> table_;
};

