#pragma once

#include "LGF/Collision/ColliderWorld.h"

#include "Stage/StageObject.h"

// =========================================================
// ステージを構成するオブジェクト・コライダーの管理クラス
// =========================================================
class StageManager {
public:
	StageManager(const std::string& jsonFilePath);
	~StageManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="colliderWorld"></param>
	void Initialize(const std::string& jsonFilePath);

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

	// ステージオブジェクトのリスト
	std::vector<std::unique_ptr<StageObject>> objects_;

	// コライダーハンドル
	LGF::Collision::ColliderHandle colliderHandleInteractionSensor_; // プレイヤーのインタラクト用センサー
};
