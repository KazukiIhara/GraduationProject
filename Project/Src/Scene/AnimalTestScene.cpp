#include "AnimalTestScene.h"

AnimalTestScene::AnimalTestScene(const InitData& init)
	: SampleScene(init){
	rabbit_ = std::make_unique<Rabbit>();
	rabbit_->Init();
}

void AnimalTestScene::Update() {
	rabbit_->Update();
}

void AnimalTestScene::Draw() const {
	DrawStage();
	rabbit_->Draw();
}
