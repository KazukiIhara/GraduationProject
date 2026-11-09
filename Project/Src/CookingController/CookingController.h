#pragma once

#include "Wok.h"
#include "Ladle.h"
#include "LGF/Physics.h"

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
	explicit CookingController(LGF::Physics::PhysicsWorld& physicsWorld);

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

	/// <summary>
	/// 中華鍋を取得
	/// </summary>
	const Wok& GetWok() const;

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// 調理器具クラス
	std::unique_ptr<Wok> wok_;
	std::unique_ptr<Ladle> ladle_;
};

