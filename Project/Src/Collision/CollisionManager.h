#pragma once

#include "LGF/Collision/ColliderWorld.h"

// =========================================================
// コリジョンマネージャー
// =========================================================
class CollisionManager {
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static CollisionManager* GetInstance();

	/* ワールド・タグの操作 */

	/// <summary>
	/// タグの登録
	/// </summary>
	/// <param name="tag"></param>
	/// <returns></returns>
	bool RegisterTag(const LGF::Collision::CollisionTag& tag);

	/// <summary>
	/// コライダーとタグのリセット（シーン切り替え時など）
	/// </summary>
	void Clear();

	/* コライダーの登録・解除 */

	/// <summary>
	/// コライダーを追加
	/// </summary>
	/// <param name="shape"></param>
	/// <param name="options"></param>
	/// <returns></returns>
	LGF::Collision::ColliderHandle AddCollider(const LGF::Collision::ColliderShape& shape, const LGF::Collision::ColliderOptions& options = {});

	/// <summary>
	/// コライダー削除
	/// </summary>
	bool RemoveCollider(LGF::Collision::ColliderHandle handle);

	/* 状態の更新・設定 */

	/// <summary>
	/// コライダーの形状を更新
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="shape"></param>
	/// <returns></returns>
	bool SetShape(LGF::Collision::ColliderHandle handle, const LGF::Collision::ColliderShape& shape);

	/// <summary>
	/// コライダーの有効化/無効化
	/// </summary>
	/// <param name="handle"></param>
	/// <param name="enabled"></param>
	/// <returns></returns>
	bool SetEnabled(LGF::Collision::ColliderHandle handle, bool enabled);

	/// <summary>
	/// コライダー情報の取得
	/// </summary>
	/// <param name="handle"></param>
	/// <returns></returns>
	std::optional<LGF::Collision::ColliderState> GetColliderState(LGF::Collision::ColliderHandle handle) const;

	/* シーン用ループ処理・イベント取得 */

	/// <summary>
	/// 全コライダー更新
	/// </summary>
	void Step();

	/// <summary>
	/// 指定したコライダーの衝突イベントを取得
	/// </summary>
	/// <param name="handle"></param>
	/// <returns></returns>
	LGF::Collision::ColliderEvents GetEvents(LGF::Collision::ColliderHandle handle);

	/// <summary>
	/// レイキャスト
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="maxDistance"></param>
	/// <param name="tags"></param>
	/// <returns></returns>
	std::optional<LGF::Collision::WorldRaycastHit> RaycastClosest(
		const LGF::Collision::Ray& ray, 
		float maxDistance = std::numeric_limits<float>::infinity(), 
		const LGF::Collision::CollisionTagList& tags = {}
	) const;

	/* デバッグ機能 */

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void DrawImGuiDebug();

private:
	// コライダーの一括管理クラス
	LGF::Collision::ColliderWorld colliderWorld_;

	// コライダーハンドルのリスト（デバッグ表示用）
	std::vector<LGF::Collision::ColliderHandle> debugColliderHandles_;
};
