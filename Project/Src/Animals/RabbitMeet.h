#pragma once
#include "BaseDropFood.h"

/// <summary>
/// 具材 - 兎肉 -
/// </summary>
class RabbitMeet : public BaseDropFood {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="bornPosition">召喚位置</param>
	void Init(LGF::Vector3 bornPosition) override;

};

