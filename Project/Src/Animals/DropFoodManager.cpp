#include "DropFoodManager.h"

#include "RabbitMeet.h"

DropFoodManager* DropFoodManager::sInstance_ = nullptr;

DropFoodManager* DropFoodManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = new DropFoodManager();
	}
	
	return sInstance_;
}

DropFoodManager::~DropFoodManager() {
	delete sInstance_;
}

void DropFoodManager::Update() {
	for (auto& dropFood : dropFoods_) {
		dropFood->Update();
	}
}

void DropFoodManager::Draw() {
	for (auto& dropFood : dropFoods_) {
		dropFood->Draw();
	}
}

void DropFoodManager::ActivationDropFood(LGF::Vector3 position) {
	std::unique_ptr<BaseDropFood> newDropFood;
	
	newDropFood = std::make_unique<RabbitMeet>();
	newDropFood->Init(position);
	dropFoods_.push_back(std::move(newDropFood));
}

