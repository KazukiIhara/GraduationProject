#pragma once

#include "GameApp.h"

#include <cstdint>
#include <functional>
#include <string>

class UIWorldDemoScene final : public GameApp::Scene {
public:
	explicit UIWorldDemoScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	LGF::UIButtonHandle AddButton(
		std::string text,
		float positionY,
		bool cursorEnabled,
		bool navigationEnabled,
		std::function<bool()> isDirectPressed = {});

	LGF::Font2D titleFont_;
	LGF::Font2D buttonFont_;
	LGF::Font2D bodyFont_;
	LGF::Font2D captionFont_;

	LGF::InputCommand previousCommand_{ "UI.Demo.Previous" };
	LGF::InputCommand nextCommand_{ "UI.Demo.Next" };
	LGF::InputCommand submitCommand_{ "UI.Demo.Submit" };
	LGF::InputCommand pointerSubmitCommand_{ "UI.Demo.PointerSubmit" };
	LGF::InputCommand directButtonCommand_{ "UI.Demo.DirectButton" };
	LGF::InputCommand toggleHitboxesCommand_{ "UI.Demo.ToggleHitboxes" };

	LGF::UIWorld uiWorld_;
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
