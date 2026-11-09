#pragma once
#include "GameApp.h"
#include "BaseDropFood.h"

/// <summary>
/// 動物が落とす具材召喚マネージャ
/// </summary>
class DropFoodManager {
public:

	static DropFoodManager* GetInstance();

	~DropFoodManager();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// 倒した後、食材が出る
	/// </summary>
	/// <param name="position">倒した動物の位置</param>
	void ActivationDropFood(LGF::Vector3 position);


private:

	static DropFoodManager* sInstance_;

	std::list <std::unique_ptr<BaseDropFood>> dropFoods_;

};

