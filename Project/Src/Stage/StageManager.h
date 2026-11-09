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
	void Initialize();

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
	// =========================================================
	// Constants
	// =========================================================

	const LGF::Vector3 kColliderChineseRangeMin = {-1.0f, -1.0f, -1.0f};
	const LGF::Vector3 kColliderChineseRangeMax = {1.0f, 1.0f, 1.0f};

	// =========================================================
	// Member Variables
	// =========================================================

	// 位置
	LGF::Vector3 positionChineseRange_ = {0.0f, 1.0f, 0.0f};

	// モデル
	LGF::Model modelGround_{"../Assets/Models/environment/ground/ground.gltf"}; // 地面
	LGF::Model modelTree_01_{"../Assets/Models/environment/tree/tree_01.gltf"};  // 木01
	LGF::Model modelTree_02_{"../Assets/Models/environment/tree/tree_02.gltf"}; // 木02
	LGF::Model modelRock_01_{"../Assets/Models/environment/rock/rock_01.gltf"}; // 岩01
	LGF::Model modelGrass_01_{"../Assets/Models/environment/grass/grass_01.gltf"}; // 草01
	LGF::Model modelChineseRange_{"../Assets/Models/chinese_range.gltf"}; // 中華レンジ

	// コライダーハンドル
	LGF::Collision::ColliderHandle colliderHandleGround_; // 地面
	LGF::Collision::ColliderHandle colliderHandleChineseRange_; // 中華レンジ
	LGF::Collision::ColliderHandle colliderHandleInteractionSensor_; // プレイヤーのインタラクト用センサー
};
