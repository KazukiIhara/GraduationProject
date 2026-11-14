#pragma once

#include "GameApp.h"

#include <string>
#include <vector>

class SkinningAnimationDemoScene final : public GameApp::Scene {
public:
	explicit SkinningAnimationDemoScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	LGF::Model foxModel_;
	LGF::ModelAnimator walkAnimator_;
	LGF::ModelAnimator surveyAnimator_;
	LGF::ModelAnimator runAnimator_;
	LGF::DebugCamera3D camera_;
	LGF::DirectionalLight directionalLight_;
	LGF::Skybox skybox_;
	LGF::Ground3D ground_;
	LGF::Font2D titleFont_;
	LGF::Font2D captionFont_;
	std::vector<LGF::ModelInstance3D> walkInstances_;
	std::string animationNames_;
};
