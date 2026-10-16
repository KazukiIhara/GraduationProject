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

	/// <summary>
	/// 召喚処理
	/// </summary>
	void BornAnimals();

private:

	std::unique_ptr<BaseAnimal> RandomAnimalsClass(int a);

private:

	/// <summary>
	/// 動物たちの番号
	/// </summary>
	enum AnimalLevel {
		rabbit,   //兎
		deer,     //鹿
		chicken,  //鶏
		pig,      //豚
		boar      //猪
	};

	//動物たちクラス
	std::list<std::unique_ptr<BaseAnimal>> animals_;

	int charhanLevel_ = 1;//レベルごとに登場する動物が変化

	const uint32_t kMaxAnimalSpawn_ = 20;//動物最大配置数

	// -- ↓ 試し　プレイヤーできたら消す --


	LGF::Vector3 charHanPos = { 0,1.0f,0 };//ターゲット位置
	LGF::Vector3 playerPos = { 2.0f,1.0f,0 };//ターゲット位置

	LGF::Box3D targeetPointer_{ 0.5f,2.0f,0.5f };//可視化ターゲット
	LGF::Box3D playerPointer_{ 1.0f,1.5f,1.0f };//可視化プレイヤー

	float playerArea_ = 5.0f;//プレイヤーの間合い
	float charHanArea_ = 5.0f;//炒飯の間合い

	//操作キー
	std::unique_ptr<LGF::KeyButton> keyA_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::A);
	std::unique_ptr<LGF::KeyButton> keyD_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::D);
	std::unique_ptr<LGF::KeyButton> keyW_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::W);
	std::unique_ptr<LGF::KeyButton> keyS_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::S);
};

