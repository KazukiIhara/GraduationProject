#pragma once

#include "LGF/Collision/ColliderWorld.h"
#include "LGF/Physics.h"
#include "Stage/StageObject.h"

// =========================================================
// ステージを構成するオブジェクト・コライダーの管理クラス
// =========================================================
class StageManager {
public:
	StageManager(const std::string& jsonFilePath, LGF::Physics::PhysicsWorld& physicsWorld);
	~StageManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="colliderWorld"></param>
	void Initialize(const std::string& jsonFilePath, LGF::Physics::PhysicsWorld& physicsWorld);

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

	/* JSON読み込み用 */

	/// <summary>
	/// モデルのあるパスを取得
	/// </summary>
	/// <param name="type"></param>
	/// <param name="name"></param>
	/// <returns></returns>
	std::string GetModelPath(const std::string& type, const std::string& name);

private:
	// =========================================================
	// Constants
	// =========================================================


	// =========================================================
	// Member Variables
	// =========================================================

	LGF::Physics::PhysicsWorld& physicsWorld_;

	// ステージオブジェクトのリスト
	std::vector<std::unique_ptr<StageObject>> objects_;

	// 部類オブジェクトハンドル
	std::vector<LGF::Physics::StaticObjectHandle> staticPhysicsHandles_;
	
	// 物理ワールドから取得した静的オブジェクトの状態を保持（デバッグ表示に使用）
	std::vector<LGF::Physics::StaticObjectState> staticObjectStates_;
	
	// プレイヤーのインタラクト用センサー
	LGF::Collision::ColliderHandle colliderHandleInteractionSensor_;
};
