#include "SpawnAnimals.h"
#include <random>

#include "Animals/Rabbit/Rabbit.h"
#include "Animals/Deer/Deer.h"
#include "Animals/Boar/Boar.h"

#include <imgui.h>

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
		}
	);


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



	std::unique_ptr<BaseAnimal> newAnimal = std::move(RandomAnimalsClass(distLevel(device)));

	if (newAnimal) {
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

		newAnimal->Init({ positionX,1.0f,positionZ });
		animals_.push_back(std::move(newAnimal));
	}
}

void SpawnAnimals::BornAnimalsDebug() {

#ifdef _DEBUG
	std::unique_ptr<BaseAnimal> newAnimal_Debug;
	std::random_device device_Debug;

	if (ImGui::Begin("SpawnAnimals Debug")) {
		if (ImGui::Button("Rabbit Spawn")) {
			newAnimal_Debug = std::move(RandomAnimalsClass(0));
		}
		if (ImGui::Button("Deer Spawn")) {
			newAnimal_Debug = std::move(RandomAnimalsClass(1));
		}
		if (ImGui::Button("Boar Spawn")) {
			newAnimal_Debug = std::move(RandomAnimalsClass(2));
		}
	}
	ImGui::End();

	if (newAnimal_Debug) {
		std::uniform_real_distribution<float> distPos(-15.0f, 15.0f);
		std::uniform_int_distribution<int> distSpawn(0, 2);//召喚場所(0:前、1:左、2:右)

		float positionX = 0.0f;
		float positionZ = 0.0f;
		if (distSpawn(device_Debug) == 1) {
			positionX = -15.0f;
			positionZ = distPos(device_Debug);
		}
		else if (distSpawn(device_Debug) == 2) {
			positionX = 15.0f;
			positionZ = distPos(device_Debug);
		}
		else {
			positionX = distPos(device_Debug);
			positionZ = 15.0f;
		}

		newAnimal_Debug->Init({ positionX,1.0f,positionZ });
		animals_.push_back(std::move(newAnimal_Debug));
	}

#endif // _DEBUG
}

std::unique_ptr<BaseAnimal> SpawnAnimals::RandomAnimalsClass(int level) {
	//動物の種類
	if (level == AnimalLevel::rabbit) {
		return std::make_unique<Rabbit>();
	}
	else if (level == AnimalLevel::deer) {
		return std::make_unique<Deer>();
	}
	else if (level == AnimalLevel::boar) {
		return std::make_unique<Boar>();
	}

	return nullptr;
}