#include "Rabbit.h"

using namespace LGF;

void Rabbit::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 0.05f;
}

void Rabbit::Update() {
	GoToTarget();

	if (hp_ <= 0 && rotate_.x <= 0.75f) {
		rotate_.x += 0.05f;
	}
}

void Rabbit::Draw() {
	object_.Draw(Math::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f },
		{ rotate_.x,rotate_.y ,rotate_.z },
		position_));
}