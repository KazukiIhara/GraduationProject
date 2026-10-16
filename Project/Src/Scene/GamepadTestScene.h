#pragma once

#include "Scene/SampleScene.h"

#include "../../Calibration.h"

class GamepadTestScene final : public SampleScene {
public:
	explicit GamepadTestScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	LGF::Box3D leftOrientationBox_{ 1.5f, 0.45f, 3.0f };
	LGF::Box3D rightOrientationBox_{ 1.5f, 0.45f, 3.0f };
	LGF::Vector3 leftRotation_{};
	LGF::Vector3 rightRotation_{};
	bool wasLeftConnected_ = false;
	bool wasRightConnected_ = false;

	std::unique_ptr<Calibration> calibration_ = nullptr;
};
