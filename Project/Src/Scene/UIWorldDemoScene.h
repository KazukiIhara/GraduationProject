#pragma once

#include "GameApp.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class UIWorldDemoScene final : public GameApp::Scene {
public:
	explicit UIWorldDemoScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	struct DemoButton {
		LGF::UIButtonHandle handle{};
		std::string text;
	};

	LGF::UIButtonHandle AddButton2D(
		std::string text,
		const LGF::Vector2& position,
		bool cursorEnabled,
		bool navigationEnabled,
		std::function<bool()> isDirectPressed = {});
	LGF::UIButtonHandle AddButton3D(
		std::string text,
		float positionY,
		bool cursorEnabled,
		bool navigationEnabled,
		std::function<bool()> isDirectPressed = {});

	LGF::Font2D titleFont_;
	LGF::Font2D buttonFont2D_;
	LGF::Font2D bodyFont_;
	LGF::Font2D captionFont_;
	LGF::Font3D buttonFont3D_;
	LGF::DebugCamera3D camera_{ 8.0f };
	LGF::DirectionalLight light_;
	LGF::Grid3D grid_{ 16.0f, 1.0f };
	LGF::UIPanel3D buttonPanel_;

	LGF::InputCommand previousCommand_{ "UI.Demo.Previous" };
	LGF::InputCommand nextCommand_{ "UI.Demo.Next" };
	LGF::InputCommand submitCommand_{ "UI.Demo.Submit" };
	LGF::InputCommand pointerSubmitCommand_{ "UI.Demo.PointerSubmit" };
	LGF::InputCommand directButtonCommand_{ "UI.Demo.DirectButton" };
	LGF::InputCommand toggleHitboxesCommand_{ "UI.Demo.ToggleHitboxes" };

	LGF::UIWorld uiWorld_;
	std::vector<DemoButton> buttons_;
	LGF::Vector2 cursorPosition_{};
	std::string triggerMessage_ = "Trigger: -";
	std::string holdMessage_ = "Hold: -";
	std::string releaseMessage_ = "Release: -";
	uint32_t triggerCount_{};
	uint32_t holdCount_{};
	uint32_t releaseCount_{};
	bool isPadCursor_{};
	bool areHitboxesVisible_{};
};
