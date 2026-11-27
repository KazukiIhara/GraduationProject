#include "AnimalTestScene.h"
#include "Animals/SetUp/DropFoodManager.h"
#include "Collision/CollisionManager.h"

AnimalTestScene::AnimalTestScene(const InitData& init)
	: SampleScene(init){

	auto* collisionManager = CollisionManager::GetInstance();
	
	collisionManager->Clear();
	// タグの登録
	collisionManager->RegisterTag("Player"); // プレイヤー
	collisionManager->RegisterTag("Solid"); // 固体（プレイヤーとの押し戻しを行うオブジェクト）
	collisionManager->RegisterTag("InteractionSensor"); // インタラクト用センサー（プレイヤーの中華レンジ周辺でのモード切替用）
	
	collisionManager->RegisterTag("Rabbit_body");
	collisionManager->RegisterTag("Rabbit_attackArea");
	collisionManager->RegisterTag("Boar_body");
	collisionManager->RegisterTag("Boar_attackArea");
	collisionManager->RegisterTag("Deer_body");
	collisionManager->RegisterTag("Deer_attackArea");

	spawnAnimals_ = std::make_unique<SpawnAnimals>();

}

void AnimalTestScene::Update() {

	spawnAnimals_->Update();

	DropFoodManager::GetInstance()->Update();

	CollisionManager::GetInstance()->Step();

#ifdef _DEBUG

	CollisionManager::GetInstance()->DrawImGuiDebug();

#endif // _DEBUG


}

void AnimalTestScene::Draw() const {
	DrawStage();
	spawnAnimals_->Draw();
	DropFoodManager::GetInstance()->Draw();
}
