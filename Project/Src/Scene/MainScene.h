#pragma once

#include "GameApp.h"

class MainScene final : public GameApp::Scene {
public:
	explicit MainScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	LGF::DebugCamera3D camera_;
	LGF::DirectionalLight light_;
	LGF::Grid3D grid_{ 20.0f, 1.0f };
	LGF::Box3D box_{ 2.0f };
	float rotation_ = 0.0f;
};
