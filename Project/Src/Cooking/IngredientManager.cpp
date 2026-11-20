#include "IngredientManager.h"

#include "LGF/Input.h"
#include "Input/XboxController.h"

#include <imgui.h>

using namespace LGF;

namespace {

	constexpr std::size_t kRiceCountPerServing = 32u;
	constexpr float kRiceAmountPerServing = 150.0f;

}

IngredientManager::IngredientManager(Physics::PhysicsWorld& physicsWorld) :
	physicsWorld_(&physicsWorld) {
	riceRenderGroup_ = physicsWorld_->CreateParticleRenderGroup({
		.color = { 0.95f, 0.92f, 0.78f, 1.0f },
	});
}

void IngredientManager::Update(const Vector3& riceSpawnPosition) {
	const auto xboxController = XboxController::Get();
	const bool isXboxAddRiceTriggered = xboxController.has_value() &&
		xboxController->Button(GamepadButtonCode::West).Trigger();
	if (Key::Space.Trigger() || isXboxAddRiceTriggered) {
		AddRice(riceSpawnPosition);
	}
}

void IngredientManager::Draw() const {
	physicsWorld_->DrawParticleRenderGroups();
}

void IngredientManager::DrawImGuiDebug() {
#ifdef _DEBUG
	if (ImGui::Begin("Cooking Fried Rice")) {
		ImGui::Text("Space / Xbox X : Add Rice");
		ImGui::Text("Rice Amount : %.0f g", cookingFriedRice_.GetRiceAmount());
		ImGui::Text("Rice Particles : %zu", riceObjects_.size());
		ImGui::Text("Ingredients Count : %zu", cookingFriedRice_.GetIngredients().size());

		// 投入済み食材の一覧表示
		if (!cookingFriedRice_.GetIngredients().empty()) {
			ImGui::Text("Ingredients List:");
			for (const auto& ing : cookingFriedRice_.GetIngredients()) {
				const char* name = "Unknown";
				switch (ing.type) {
				case IngredientType::Egg: name = "Egg"; break;
				case IngredientType::GreenOnion: name = "GreenOnion"; break;
				case IngredientType::Meat: name = "Meat"; break;
				}
				ImGui::BulletText("%s: %.1f", name, ing.amount);
			}
		}

		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "=== Debug Add Ingredients ===");
		if (ImGui::Button("Add Egg")) {
			AddIngredient(IngredientType::Egg, 1.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Green Onion")) {
			AddIngredient(IngredientType::GreenOnion, 1.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Meat")) {
			AddIngredient(IngredientType::Meat, 1.0f);
		}

		if (ImGui::Button("Reset Ingredients & Rice")) {
			Reset();
		}

		ImGui::Separator();
		const char* stateStr = "NotStarted";
		switch (cookingFriedRice_.GetState()) {
		case FriedRiceState::NotStarted: stateStr = "NotStarted"; break;
		case FriedRiceState::Cooking: stateStr = "Cooking"; break;
		case FriedRiceState::Burned: stateStr = "Burned (Fail)"; break;
		case FriedRiceState::Completed: stateStr = "Completed"; break;
		}
		ImGui::Text("State : %s", stateStr);
	}
	ImGui::End();
#endif
}

const CookingFriedRice& IngredientManager::GetCookingFriedRice() const {
	return cookingFriedRice_;
}

CookingFriedRice& IngredientManager::GetCookingFriedRice() {
	return cookingFriedRice_;
}

void IngredientManager::AddIngredient(IngredientType type, float amount) {
	cookingFriedRice_.AddIngredient(type, amount);
}

void IngredientManager::Reset() {
	riceObjects_.clear();
	cookingFriedRice_.Reset();
}

void IngredientManager::AddRice(const Vector3& spawnPosition) {
	if (physicsWorld_ == nullptr) {
		return;
	}

	for (std::size_t index = 0u; index < kRiceCountPerServing; ++index) {
		const float horizontalIndex = static_cast<float>(index % 4u) - 1.5f;
		const float depthIndex = static_cast<float>((index / 4u) % 4u) - 1.5f;
		const float height = static_cast<float>(index / 16u) * 0.16f;
		const Vector3 position = spawnPosition + Vector3{
			horizontalIndex * 0.12f,
			height,
			depthIndex * 0.12f,
		};

		riceObjects_.push_back(std::make_unique<Rice>(
			*physicsWorld_,
			position,
			ObjectHandle{ nextObjectIndex_++, 1u },
			riceRenderGroup_));
	}

	cookingFriedRice_.AddRice(kRiceAmountPerServing);
}
