#include "SpawnAnimals.h"
#include <random>

void SpawnAnimals::Update() {

	if (keyA_->Press()) {
		playerPos.x -= 0.5f;
	}
	else if (keyD_->Press()) {
		playerPos.x += 0.5f;
	}

	if (keyW_->Press()) {
		playerPos.z += 0.5f;
	}
	else if (keyS_->Press()) {
		playerPos.z -= 0.5f;
	}


	for (auto& animal : animals_) {
		animal->SetPlayerPosition(playerPos);
		animal->Update();
		animal->Dead();
	}

	animals_.remove_if([](std::unique_ptr<BaseAnimal>& animal) {
		if (animal->IsDelete()) {
			animal.reset();
			return true;
		}
		return false;
		}
	);


}

void SpawnAnimals::Draw() {
	for (auto& animal : animals_) {
		animal->Draw();
	}

	//ターゲット位置
	targeetPointer_.Draw(LGF::Math::MakeAffineMatrix({ 0.5f,2.0f,0.5f }, { 0,0,0 }, playerPos));
}

void SpawnAnimals::BornAnimals(std::unique_ptr<BaseAnimal> animalClass) {
	//店員オーバー
	if (animals_.size() > 10) {
		return;
	}

	std::random_device device;
	std::uniform_real_distribution<float> distPosX(-10.0f, 10.0f);

	std::unique_ptr<BaseAnimal> newAnimal = std::move(animalClass);
	newAnimal->Init({ distPosX(device),1.0f,15.0f });

	animals_.push_back(std::move(newAnimal));
}
