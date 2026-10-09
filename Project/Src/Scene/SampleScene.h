#pragma once

#include "GameApp.h"

class SampleScene : public GameApp::Scene {
public:
	explicit SampleScene(const InitData& init);

protected:
	void UpdateCamera();
	void DrawStage() const;

private:
	LGF::DebugCamera3D camera_;
	LGF::DirectionalLight light_;
	LGF::Ground3D ground_;
	LGF::Grid3D grid_;
};
