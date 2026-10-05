#include "Scene/MainScene.h"

using namespace LGF;

MainScene::MainScene(const InitData& init) :
	IScene(init),
	camera_(15.0f),
	light_({ 0.5f, -1.0f, 0.35f }, Color::White, 1.5f) {
	camera_.pitch = 20.0f * Math::DEG_TO_RAD;
}

void MainScene::Update() {
	rotation_ += static_cast<float>(System::DeltaTime());
	camera_.Update();
}

void MainScene::Draw() const {
	camera_.SetScene();
	light_.SetScene();
	grid_.Draw(Math::MakeTranslateMatrix({ 0.0f, 0.01f, 0.0f }));
	box_.Draw(
		Math::MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f },
			Math::MakeRotateAxisAngle({ 0.0f, 1.0f, 0.0f }, rotation_),
			{ 0.0f, 1.0f, 0.0f }),
		Color::RoyalBlue);
}
