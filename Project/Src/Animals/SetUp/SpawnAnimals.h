#pragma once
#include <vector>
#include "Animals/Base/BaseAnimal.h"
#include "LGF/Input.h"

#include "Animals/Rabbit/Rabbit.h"
#include "Animals/Deer/Deer.h"
#include "Animals/Boar/Boar.h"

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

	/// <summary>
	/// 召喚処理(debug)
	/// </summary>
	void BornAnimalsDebug();

private:

	void RandomAnimalsClass(int a, std::random_device& device);

private:

	/// <summary>
	/// 動物たちの番号
	/// </summary>
	enum AnimalLevel {
		rabbit,   //兎
		deer,     //鹿
		boar      //猪
	};

	struct AnimalData {
		std::string name;
		std::function<std::unique_ptr<BaseAnimal>()> animalClass;
	};

	std::unordered_map<int, AnimalData> animalData_ = {
		{ AnimalLevel::rabbit, AnimalData{ "Rabbit", []() { return std::make_unique<Rabbit>(); } } },
		{ AnimalLevel::deer, AnimalData{ "Deer", []() { return std::make_unique<Deer>(); } } },
		{ AnimalLevel::boar, AnimalData{ "Boar", []() { return std::make_unique<Boar>(); } } },
	};

	//動物たちクラス
	std::list<std::unique_ptr<BaseAnimal>> animals_;

	int charhanLevel_ = 2;//レベルごとに登場する動物が変化

	const uint32_t kMaxAnimalSpawn_ = 10;//動物最大配置数

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
	std::unique_ptr<LGF::KeyButton> keySpace_ = std::make_unique<LGF::KeyButton>(LGF::KeyCode::Space);
};

