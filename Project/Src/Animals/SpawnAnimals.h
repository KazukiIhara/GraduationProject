#pragma once
#include <vector>
#include "Animals/BaseAnimal.h"
#include "LGF/Input.h"

/// <summary>
/// 動物を召喚する
/// </summary>
class SpawnAnimals {
public:

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	void BornAnimals(std::unique_ptr<BaseAnimal> animalClass);

private:
	//動物たちクラス
	std::list<std::unique_ptr<BaseAnimal>> animals_;

	// -- ↓ 試し --

	LGF::Vector3 playerPos = { 0,1.0f,0 };//ターゲット位置
	LGF::Box3D targeetPointer_{ 0.5f,2.0f,0.5f };//可視化ターゲット

	//操作キー
	std::unique_ptr<LGF::KeyButton> keyA_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::A);
	std::unique_ptr<LGF::KeyButton> keyD_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::D);
	std::unique_ptr<LGF::KeyButton> keyW_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::W);
	std::unique_ptr<LGF::KeyButton> keyS_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::S);
};

