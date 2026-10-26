#pragma once

#include "LGF/Graphics/Model.h"
#include "LGF/Collision/ColliderWorld.h"

// =========================================================
// ステージを構成するオブジェクト・コライダーの管理クラス
// =========================================================
class StageManager {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StageManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StageManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="colliderWorld"></param>
	void Initialize(LGF::Collision::ColliderWorld* colliderWorld);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ描画処理
	/// </summary>
	void DrawImGuiDebug();

private:
	// モデル
	LGF::Model chineseRangeModel_{"../Assets/Models/chinese_range.gltf"}; // 中華レンジ

	// コライダー管理
	LGF::Collision::ColliderWorld* colliderWorld_ = nullptr;

	// コライダーハンドル
	LGF::Collision::ColliderHandle rangeColliderHandle_; // 中華レンジ
};
