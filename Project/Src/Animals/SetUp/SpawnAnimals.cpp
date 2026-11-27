#include "SpawnAnimals.h"
#include <random>

#include <imgui.h>
#include <memory>

void SpawnAnimals::Update() {

	if (keyA_->Press()) {
		playerPos.x -= 2.0f * (float)LGF::System::DeltaTime();
	}
	else if (keyD_->Press()) {
		playerPos.x += 2.0f * (float)LGF::System::DeltaTime();
	}

	if (keyW_->Press()) {
		playerPos.z += 2.0f * (float)LGF::System::DeltaTime();
	}
	else if (keyS_->Press()) {
		playerPos.z -= 2.0f * (float)LGF::System::DeltaTime();
	}


	for (auto& animal : animals_) {
		animal->SetCharHanPosition(charHanPos);
		animal->SetPlayerPosition(playerPos);
		if (keySpace_->Press()) { 
			animal->Damage();// とりあえずspaceでダメージを食らう判定をとっている
		}
		animal->Update();
	}

	animals_.remove_if([](std::unique_ptr<BaseAnimal>& animal) {
		if (animal->IsDelete()) {
			animal.reset();
			return true;
		}
		return false; 
	});
}

void SpawnAnimals::Draw() {
	for (auto& animal : animals_) {
		animal->Draw();
	}

	//ターゲット位置
	targeetPointer_.Draw(LGF::Math::MakeAffineMatrix({ 0.5f,2.0f,0.5f }, { 0,0,0 }, charHanPos));
	playerPointer_.Draw(LGF::Math::MakeAffineMatrix({ 1.0f,1.5f,1.0f }, { 0,0,0 }, playerPos));
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