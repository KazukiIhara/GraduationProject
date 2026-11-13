#include "Scene/UIWorldDemoScene.h"

using namespace LGF;

namespace {

	constexpr const char* kFontPath = "../Assets/Fonts/NotoSansJP/NotoSansJP-wght.ttf";

}

UIWorldDemoScene::UIWorldDemoScene(const InitData& init) :
	IScene(init),
	titleFont_(kFontPath, 40u),
	buttonFont2D_(kFontPath, 20u),
	bodyFont_(kFontPath, 20u),
	captionFont_(kFontPath, 16u),
	buttonFont3D_(kFontPath, 32u),
	light_({ 0.3f, -1.0f, 0.5f }, Color::White, 1.5f) {
	buttons3D_.push_back(uiWorld_.AddButton3D({
		.text = "Cursor + Navigation",
		.transform = {
			.position = { 0.0f, 1.2f, 0.0f },
			.size = { 3.6f, 0.65f },
		},
		.isNavigationEnabled = true,
	}));
	buttons3D_.push_back(uiWorld_.AddButton3D({
		.text = "Cursor Only",
		.transform = {
			.position = { 0.0f, 0.4f, 0.0f },
			.size = { 3.6f, 0.65f },
		},
	}));
	buttons3D_.push_back(uiWorld_.AddButton3D({
		.text = "Navigation Only",
		.transform = {
			.position = { 0.0f, -0.4f, 0.0f },
			.size = { 3.6f, 0.65f },
		},
		.isCursorEnabled = false,
		.isNavigationEnabled = true,
	}));
	buttons3D_.push_back(uiWorld_.AddButton3D({
		.text = "Direct Input: 1 / Pad East",
		.transform = {
			.position = { 0.0f, -1.2f, 0.0f },
			.size = { 3.6f, 0.65f },
		},
		.directInputCommand = "UI.Demo.DirectButton",
		.isCursorEnabled = false,
	}));

	overlayButton_ = uiWorld_.AddButton({
		.text = "2D Overlay Button",
		.transform = {
			.position = { 24.0f, 184.0f },
			.size = { 240.0f, 48.0f },
		},
	});

	buttons_ = buttons3D_;
	buttons_.push_back(overlayButton_);

}

void UIWorldDemoScene::Update() {
	if (Key::Escape.Trigger()) {
		const std::string& previousScene = GetData().previousScene;
		ChangeScene(previousScene.empty()
			? SceneNames::GamePlayScene
			: previousScene);
		return;
	}

	uiPointer_.Update();
	const Vector2& cursorPosition = uiPointer_.GetPosition();

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
		.cursorPosition = cursorPosition,
		.isCursorPressed = pointerSubmitCommand_.Press(),
		.navigationOffset = navigationOffset,
		.isNavigationPressed = submitCommand_.Press(),
		.cursor3D = MakeUICursor3D(camera_, cursorPosition, 100.0f),
	});

	for (const UIButton& button : buttons_) {
		const std::string text = button.GetText();
		if (button.Trigger()) {
			++triggerCount_;
			triggerMessage_ = "Trigger: " + text +
				" (" + std::to_string(triggerCount_) + ")";
		}
		if (button.Press()) {
			++holdCount_;
			holdMessage_ = "Press: " + text +
				" (" + std::to_string(holdCount_) + ")";
		}
		if (button.Release()) {
			++releaseCount_;
			releaseMessage_ = "Release: " + text +
				" (" + std::to_string(releaseCount_) + ")";
		}
	}
}

void UIWorldDemoScene::Draw() const {
	const Window::Size windowSize = Window::GetSize();

	camera_.SetScene();
	light_.SetScene();
	grid_.Draw();
	for (const UIButton& button : buttons3D_) {
		button.Draw(buttonPanel_, buttonFont3D_);
	}
	if (areHitboxesVisible_) {
		uiWorld_.DrawHitboxes3D(Color::Lime);
	}

	titleFont_.Draw(
		"InputCommand + 2D / 3D UIWorld Demo",
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

	overlayButton_.Draw(buttonFont2D_);
	if (areHitboxesVisible_) {
		uiWorld_.DrawHitboxes({ 0.0f, 1.0f, 0.0f, 0.22f });
	}

	const float statusTop = static_cast<float>(windowSize.height) - 128.0f;
	Rect2D{ 24.0f, statusTop, 660.0f, 104.0f }
		.Draw(Vector4{ 0.04f, 0.06f, 0.1f, 0.94f });
	bodyFont_.Draw(triggerMessage_, { 42.0f, statusTop + 12.0f });
	bodyFont_.Draw(holdMessage_, { 42.0f, statusTop + 40.0f });
	bodyFont_.Draw(releaseMessage_, { 42.0f, statusTop + 68.0f });

	uiPointer_.Draw();
}
