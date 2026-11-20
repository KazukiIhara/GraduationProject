#pragma once

#include "CookingFriedRice.h"
#include "Rice.h"

#include <cstdint>
#include <memory>
#include <vector>

// =========================================================
// 食材管理クラス
// =========================================================
class IngredientManager
{
public:
	explicit IngredientManager(LGF::Physics::PhysicsWorld& physicsWorld);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="riceSpawnPosition">米を投入する位置</param>
	void Update(const LGF::Vector3& riceSpawnPosition);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const;

	/// <summary>
	/// ImGuiデバッグ描画
	/// </summary>
	void DrawImGuiDebug();

	/// <summary>
	/// 調理中の炒飯を取得
	/// </summary>
	const CookingFriedRice& GetCookingFriedRice() const;
	CookingFriedRice& GetCookingFriedRice();

	/// <summary>
	/// 食材を追加（CookingFriedRiceに登録）
	/// </summary>
	void AddIngredient(IngredientType type, float amount = 1.0f);

	/// <summary>
	/// リセット処理（米オブジェクトと炒飯状態を初期化）
	/// </summary>
	void Reset();

private:
	/// <summary>
	/// 米を一人前追加
	/// </summary>
	void AddRice(const LGF::Vector3& spawnPosition);

private:
	LGF::Physics::PhysicsWorld* physicsWorld_ = nullptr;
	LGF::Physics::PhysicsParticleRenderGroupHandle riceRenderGroup_{};
	CookingFriedRice cookingFriedRice_;
	std::vector<std::unique_ptr<Rice>> riceObjects_;
	uint32_t nextObjectIndex_ = 1000u;
};
