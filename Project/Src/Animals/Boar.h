#pragma once
#include "BaseAnimal.h"
class Boar : public BaseAnimal {
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="spownPos">召喚位置</param>
	void Init(const LGF::Vector3& spownPos) override;

	/// <summary>
	/// 移動方法
	/// </summary>
	void Move() override;
};

