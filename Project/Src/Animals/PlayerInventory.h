#pragma once
#include "GameApp.h"
#include "Base/BaseDropFood.h"

/// <summary>
/// 材料が入るインベントリ
/// </summary>
class PlayerInventory {
public:
	/// <summary>
	/// 拾う
	/// </summary>
	void PickUp();
	/// <summary>
	/// 離す
	/// </summary>
	void ReleaseFood();

	/// <summary>
	/// 炒飯に入れる
	/// </summary>
	void InCharHan();

private:

	std::list<std::unique_ptr<BaseDropFood>> dropFoods_;
};
