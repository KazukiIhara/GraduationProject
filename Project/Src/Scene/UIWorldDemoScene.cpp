#include "Scene/UIWorldDemoScene.h"

#include <algorithm>
#include <cmath>
#include <utility>

using namespace LGF;

namespace {

	constexpr const char* kFontPath = "../Assets/Fonts/NotoSansJP/NotoSansJP-wght.ttf";

}

UIWorldDemoScene::UIWorldDemoScene(const InitData& init) :
	IScene(init),
	titleFont_(kFontPath, 40u),
	buttonFont_(kFontPath, 24u),
	bodyFont_(kFontPath, 20u),
	captionFont_(kFontPath, 16u) {
	AddButton("Cursor + Navigation", -108.0f, true, true);
	AddButton("Cursor Only", -36.0f, true, false);
	AddButton("Navigation Only", 36.0f, false, true);
	AddButton(
		"Direct Input: 1 / Pad East",
		108.0f,
		false,
		false,
		[this] {
			return directButtonCommand_.Press();
		});

	const Window::Size windowSize = Window::GetSize();
	cursorPosition_ = {
		static_cast<float>(windowSize.width) * 0.5f,
		static_cast<float>(windowSize.height) * 0.5f,
	};
}

void UIWorldDemoScene::Update() {
	if (Key::Escape.Trigger()) {
		const std::string& previousScene = GetData().previousScene;
		ChangeScene(previousScene.empty()
			? SceneNames::GamePlayScene
			: previousScene);
		return;
	}

	const Gamepad gamepad{ 0u };
	const Vector2 stick = gamepad.LeftStick();
	constexpr float kStickDeadZone = 0.18f;
	constexpr float kCursorSpeed = 720.0f;
	if (std::abs(stick.x) >= kStickDeadZone ||
		std::abs(stick.y) >= kStickDeadZone) {
		isPadCursor_ = true;
		const float deltaTime = static_cast<float>(System::DeltaTime());
		cursorPosition_ += Vector2{ stick.x, -stick.y } *
			(kCursorSpeed * deltaTime);
	}

	const Vector2 mouseCursorDelta = Cursor::Delta();
	if (mouseCursorDelta.x != 0.0f || mouseCursorDelta.y != 0.0f) {
		isPadCursor_ = false;
	}
	if (!isPadCursor_) {
		cursorPosition_ = Cursor::Pos();
	}

	const Window::Size windowSize = Window::GetSize();
	cursorPosition_.x = std::clamp(
		cursorPosition_.x,
		0.0f,
		static_cast<float>(windowSize.width));
	cursorPosition_.y = std::clamp(
		cursorPosition_.y,
		0.0f,
		static_cast<float>(windowSize.height));

	int32_t navigationOffset = 0;
	if (previousCommand_.Trigger()) {
		navigationOffset = -1;
	} else if (nextCommand_.Trigger()) {
		navigationOffset = 1;
	}
	if (toggleHitboxesCommand_.Trigger()) {
		areHitboxesVisible_ = !areHitboxesVisible_;
	}

	uiWorld_.Update({
		.cursorPosition = cursorPosition_,
		.isCursorPressed = pointerSubmitCommand_.Press(),
		.navigationOffset = navigationOffset,
		.isNavigationPressed = submitCommand_.Press(),
	});
}

void UIWorldDemoScene::Draw() const {
	const Window::Size windowSize = Window::GetSize();
	Rect2D{
		0.0f,
		0.0f,
		static_cast<float>(windowSize.width),
		static_cast<float>(windowSize.height),
	}.Draw(Vector4{ 0.015f, 0.02f, 0.04f, 1.0f });

	titleFont_.Draw(
		"InputCommand + UIWorld Demo",
		{ static_cast<float>(windowSize.width) * 0.5f, 28.0f },
		{
			.color = Color::AliceBlue,
			.anchor = { 0.5f, 0.0f },
		});
	captionFont_.Draw(
		"Escape: Return    Mouse: Move / Left Click    Pad Cursor: Left Stick / Right Shoulder",
		{ 24.0f, 82.0f },
		{ .color = Color::LightGray });
	captionFont_.Draw(
		"Selection: W/S, Up/Down, Enter/Space    Pad: D-Pad / South",
		{ 24.0f, 108.0f },
		{ .color = Color::LightGray });
	captionFont_.Draw(
		"Direct: 1 / Pad East    Hitbox: F / Pad North",
		{ 24.0f, 134.0f },
		{ .color = Color::LightGray });

	uiWorld_.Draw();
	if (areHitboxesVisible_) {
		uiWorld_.DrawHitboxes({ 0.0f, 1.0f, 0.0f, 0.22f });
	}

	const float statusTop = static_cast<float>(windowSize.height) - 128.0f;
	Rect2D{ 24.0f, statusTop, 660.0f, 104.0f }
		.Draw(Vector4{ 0.04f, 0.06f, 0.1f, 0.94f });
	bodyFont_.Draw(triggerMessage_, { 42.0f, statusTop + 12.0f });
	bodyFont_.Draw(holdMessage_, { 42.0f, statusTop + 40.0f });
	bodyFont_.Draw(releaseMessage_, { 42.0f, statusTop + 68.0f });

	const Vector4 cursorColor = isPadCursor_ ? Color::HotPink : Color::Cyan;
	Rect2D{
		cursorPosition_ - Vector2{ 7.0f, 7.0f },
		{ 14.0f, 14.0f },
	}.Draw(cursorColor);
}

UIButtonHandle UIWorldDemoScene::AddButton(
	std::string text,
	float positionY,
	bool cursorEnabled,
	bool navigationEnabled,
	std::function<bool()> isDirectPressed) {
	UIButtonDesc desc{};
	desc.transform = {
		.position = { 0.0f, positionY },
		.size = { 440.0f, 58.0f },
		.anchor = { 0.5f, 0.5f },
		.pivot = { 0.5f, 0.5f },
	};
	desc.callbacks = {
		.onTrigger = [this, text] {
			++triggerCount_;
			triggerMessage_ = "Trigger: " + text +
				" (" + std::to_string(triggerCount_) + ")";
		},
		.onHold = [this, text] {
			++holdCount_;
			holdMessage_ = "Hold: " + text +
				" (" + std::to_string(holdCount_) + ")";
		},
		.onRelease = [this, text] {
			++releaseCount_;
			releaseMessage_ = "Release: " + text +
				" (" + std::to_string(releaseCount_) + ")";
		},
	};
	desc.draw = [this, text](const UIButtonDrawContext& context) {
		Vector4 color{ 0.08f, 0.10f, 0.16f, 0.96f };
		if (context.state.isHovered) {
			color = Color::RoyalBlue;
		}
		if (context.state.isSelected) {
			color = Color::DarkOrange;
		}
		if (context.state.isHeld) {
			color = Color::HotPink;
		}

		context.rect.Draw(color);
		buttonFont_.Draw(
			text,
			context.rect.leftTop + context.rect.size * 0.5f,
			{
				.color = Color::White,
				.anchor = { 0.5f, 0.5f },
			});
	};
	desc.isDirectPressed = std::move(isDirectPressed);
	desc.isCursorEnabled = cursorEnabled;
	desc.isNavigationEnabled = navigationEnabled;
	return uiWorld_.AddButton(desc);
}
