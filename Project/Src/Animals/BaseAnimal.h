#pragma once
#include "GameApp.h"
class BaseAnimal {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// Setter_プレイヤー位置
	/// </summary>
	/// <param name="playerPos">現在のプレイヤーの位置を設定</param>
	void SetPlayerPosition(const LGF::Vector3& playerPos);

protected:
	LGF::Box3D object_;



};

