#include "CookingFriedRice.h"

#include <algorithm>

void CookingFriedRice::AddRice(float amount) {
	if (amount <= 0.0f) {
		return;
	}

	riceAmount_ += amount;
	if (state_ == FriedRiceState::NotStarted) {
		state_ = FriedRiceState::Cooking;
	}
}

void CookingFriedRice::AddIngredient(IngredientType type, float amount) {
	if (amount <= 0.0f) {
		return;
	}

	auto ingredient = std::find_if(
		ingredients_.begin(),
		ingredients_.end(),
		[type](const IngredientData& data) {
			return data.type == type;
		});

	if (ingredient != ingredients_.end()) {
		ingredient->amount += amount;
		return;
	}

	ingredients_.push_back({ type, amount });
}

float CookingFriedRice::GetRiceAmount() const {
	return riceAmount_;
}

const std::vector<IngredientData>& CookingFriedRice::GetIngredients() const {
	return ingredients_;
}

FriedRiceState CookingFriedRice::GetState() const {
	return state_;
}

void CookingFriedRice::SetState(FriedRiceState state) {
	state_ = state;
}
