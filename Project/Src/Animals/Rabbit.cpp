#include "Rabbit.h"

using namespace LGF;

void Rabbit::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 0.05f;
	hp_ = 600;
}

void Rabbit::Update() {
	BaseAnimal::Update();
	hp_--;
}

void Rabbit::Draw() {

	Matrix4x4 translate = Math::MakeTranslateMatrix(position_);
	Matrix4x4 rotate = Math::MakeRotateXMatrix(rotate_.x) * Math::MakeRotateYMatrix(rotate_.y) * Math::MakeRotateZMatrix(rotate_.z);
	Matrix4x4 scale = Math::MakeScaleMatrix({1,1,1});

	object_.Draw(scale * rotate * translate, Vector4{ 0.0f,1.0f,0.0f,1.0f });
}