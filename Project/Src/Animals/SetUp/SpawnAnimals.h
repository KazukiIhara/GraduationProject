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
	void Update(const LGF::Vector3& playerPosition, const LGF::Vector3& charHanPosition);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:

	/// <summary>
	/// 召喚処理
	/// </summary>
	void BornAnimals();

	/// <summary>
	/// 召喚処理(debug)
	/// </summary>
	void BornAnimalsDebug();

	
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

	//デバッグ版処理(任意に敵を召喚できる処理)
	bool isDebug = false;

	// -- ↓ 試し　プレイヤーできたら消す --

	float playerArea_ = 5.0f;//プレイヤーの間合い
	float charHanArea_ = 5.0f;//炒飯の間合い
};

