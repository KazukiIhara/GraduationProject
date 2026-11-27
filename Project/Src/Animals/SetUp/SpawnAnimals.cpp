#include "SpawnAnimals.h"
#include <random>

#include <imgui.h>
#include <memory>

void SpawnAnimals::Update(const LGF::Vector3& playerPosition, const LGF::Vector3& charHanPosition) {

#ifdef _DEBUG

	isDebug = true;

#endif // _DEBUG

	if (isDebug) {
		BornAnimalsDebug();
	}
	else {
		BornAnimals();
	}

	for (auto& animal : animals_) {
		animal->SetCharHanPosition(charHanPosition);
		animal->SetPlayerPosition(playerPosition);
		if (LGF::Key::Space.Press()) {
			animal->Damage();// とりあえずspaceでダメージを食らう判定をとっている
		}
		animal->Update();
	}

	animals_.remove_if([](std::unique_ptr<BaseAnimal>& animal) {
		if (animal->IsDelete()) {
			return true;
		}
		return false; 
	});
}

void SpawnAnimals::Draw() {
	//動物敵 描画処理
	for (auto& animal : animals_) {
		animal->Draw();
	}
}

void SpawnAnimals::BornAnimals() {
	//定員オーバー
	if (animals_.size() >= kMaxAnimalSpawn_) {
		return;
	}

	std::random_device device;
	std::uniform_int_distribution<int> distLevel(0, charhanLevel_);
	
	RandomAnimalsClass(distLevel(device),device);
}

void SpawnAnimals::BornAnimalsDebug() {

#ifdef _DEBUG
	std::unique_ptr<BaseAnimal> newAnimal_Debug;
	std::random_device device_Debug;

	if (ImGui::Begin("SpawnAnimals Debug")) {
		if (ImGui::Button("Rabbit Spawn")) {
			RandomAnimalsClass(AnimalLevel::rabbit,device_Debug);
		}
		if (ImGui::Button("Deer Spawn")) {
			RandomAnimalsClass(AnimalLevel::deer, device_Debug);
		}
		if (ImGui::Button("Boar Spawn")) {
			RandomAnimalsClass(AnimalLevel::boar, device_Debug);
		}
	}
	ImGui::End();
#endif // _DEBUG
}

void SpawnAnimals::RandomAnimalsClass(int level, std::random_device& device) {
	//動物の種類
	std::unique_ptr<BaseAnimal> newAnimal = animalData_[level].animalClass();

	std::uniform_real_distribution<float> distPos(-15.0f, 15.0f);
	std::uniform_int_distribution<int> distSpawn(0, 2);//召喚場所(0:前、1:左、2:右)

	float positionX = 0.0f;
	float positionZ = 0.0f;
	if (distSpawn(device) == 1) {
		positionX = -15.0f;
		positionZ = distPos(device);
	}
	else if (distSpawn(device) == 2) {
		positionX = 15.0f;
		positionZ = distPos(device);
	}
	else {
		positionX = distPos(device);
		positionZ = 15.0f;
	}

	newAnimal->Init(animalData_[level].name ,{ positionX,1.0f,positionZ });
	animals_.push_back(std::move(newAnimal));
}