#include "Rabbit.h"

using namespace LGF;

void Rabbit::Init() {

}

void Rabbit::Update() {
	position_.z -= kMoveSpeed_ * (1.0f / 60.0f);
}

void Rabbit::Draw() {
	object_.Draw(Math::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		position_));
}