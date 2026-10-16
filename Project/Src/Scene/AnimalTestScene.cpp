#include "AnimalTestScene.h"
#include "Animals/Rabbit.h"

AnimalTestScene::AnimalTestScene(const InitData& init)
	: SampleScene(init){
	spawnAnimals_ = std::make_unique<SpawnAnimals>();
}

void AnimalTestScene::Update() {
	spawnAnimals_->BornAnimals(std::make_unique<Rabbit>());
	spawnAnimals_->Update();
}

void AnimalTestScene::Draw() const {
	DrawStage();
	spawnAnimals_->Draw();
}
