#pragma once

#include "GameApp.h"

#include <string>
#include <vector>

class UIWorldDemoScene final : public GameApp::Scene {
public:
	explicit UIWorldDemoScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	LGF::Font2D titleFont_;
	LGF::Font2D buttonFont2D_;
	LGF::Font2D captionFont_;
	LGF::Font3D buttonFont3D_;
	LGF::DebugCamera3D camera_{ 8.0f };
	LGF::Grid3D grid_{ 16.0f, 1.0f };
	LGF::UIPanel3D buttonPanel_;

	LGF::InputCommand previousCommand_{ "UI.Demo.Previous" };
	LGF::InputCommand nextCommand_{ "UI.Demo.Next" };
	LGF::InputCommand submitCommand_{ "UI.Demo.Submit" };
	LGF::InputCommand pointerSubmitCommand_{ "UI.Demo.PointerSubmit" };
	LGF::InputCommand toggleHitboxesCommand_{ "UI.Demo.ToggleHitboxes" };

	LGF::UIWorld uiWorld_;
	LGF::UIPointer uiPointer_;
	std::vector<LGF::UIButtonHandle> buttons2D_;
	std::vector<LGF::UIButtonHandle> buttons3D_;
	std::vector<LGF::UIButtonHandle> buttons_;
	std::string message_ = "Select a 2D or 3D button";
	bool areHitboxesVisible_{};
};
