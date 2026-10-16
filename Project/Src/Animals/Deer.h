#pragma once
#include "BaseAnimal.h"

/// <summary>
/// 動物 -シカ-
/// </summary>
class Deer final : public BaseAnimal {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="spownPos">召喚位置</param>
	void Init(const LGF::Vector3& spownPos) override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

};

