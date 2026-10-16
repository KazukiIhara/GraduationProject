#pragma once

#include "Wok.h"
#include "Ladle.h"

// =========================================================
// 調理器具管理クラス
// =========================================================
class CookingController
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	CookingController();

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

	// 調理器具クラス
	std::unique_ptr<Wok> wok_;
	std::unique_ptr<Ladle> ladle_;
};

