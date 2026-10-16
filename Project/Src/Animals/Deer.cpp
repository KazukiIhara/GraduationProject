#include "Deer.h"

using namespace LGF;

void Deer::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 0.075f;
	hp_ = 400;
}

void Deer::Update() {
	BaseAnimal::Update();
	hp_--;
}

void Deer::Draw() {
	Matrix4x4 translate = Math::MakeTranslateMatrix(position_);
	Matrix4x4 rotate = Math::MakeRotateXMatrix(rotate_.x) * Math::MakeRotateYMatrix(rotate_.y) * Math::MakeRotateZMatrix(rotate_.z);
	Matrix4x4 scale = Math::MakeScaleMatrix({ 1,1,1 });

	object_.Draw(scale * rotate * translate, Vector4{ 1.0f,0.0f,0.0f,1.0f });
}