#pragma once
#include "GameApp.h"

/// <summary>
/// 動物を倒して出る具材の基盤クラス
/// </summary>
class BaseDropFood {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="bornPosition">召喚位置</param>
	virtual void Init(LGF::Vector3 bornPosition) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// プレイヤーに拾われる
	/// </summary>
	void PickUp();

	/// <summary>
	/// 拾われた
	/// </summary>
	/// <returns></returns>
	bool IsPickUp() {
		return isPickUp_;
	}

protected:

	LGF::Box3D object_;
	LGF::Vector3 position_;
	LGF::Vector3 rotate_ = {0,0,0};
	
	const float kFixPositionY_ = 1.0f;

	LGF::Vector4 color_ = LGF::Color::Brown;

private:

	const float kAnimationRotatePower_ = 0.5f;
	float animationTranslateUpDown_ = 0.0f;

	bool isPickUp_ = false;

	//パラメータ系統
	LGF::Vector3 kScale_ = { 0.75f,0.75f, 0.75f };

};
