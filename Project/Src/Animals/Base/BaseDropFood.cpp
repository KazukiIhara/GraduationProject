#include "BaseDropFood.h"
using namespace LGF;

void BaseDropFood::Update() {
	rotate_.y += kAnimationRotatePower_ * (float)System::DeltaTime();

	animationTranslateUpDown_ += (float)System::DeltaTime();
	float upDown = sin(animationTranslateUpDown_) / 2.0f;
	position_.y = upDown + kFixPositionY_;
}

void BaseDropFood::Draw() {
	Matrix4x4 worldMatrix = Math::MakeAffineMatrix(kScale_, Math::MakeQuaternion(rotate_), position_);
	object_.Draw(worldMatrix, color_);
}

