#include "Scene/UIWorldDemoScene.h"

using namespace LGF;

namespace {

	constexpr const char* kFontPath = "../Assets/Fonts/NotoSansJP/NotoSansJP-wght.ttf";

}

UIWorldDemoScene::UIWorldDemoScene(const InitData& init) :
	IScene(init),
	titleFont_(kFontPath, 40u),
	buttonFont2D_(kFontPath, 20u),
	captionFont_(kFontPath, 16u),
	buttonFont3D_(kFontPath, 32u) {
	buttons2D_.push_back(uiWorld_.AddButton({
		.text = "2D Cursor + Navigation",
		.transform = {
			.position = { 40.0f, 190.0f },
			.size = { 300.0f, 60.0f },
		},
		.isNavigationEnabled = true,
	}));
	buttons2D_.push_back(uiWorld_.AddButton({
		.text = "2D Navigation Only",
		.transform = {
			.position = { 40.0f, 270.0f },
			.size = { 300.0f, 60.0f },
		},
		.isCursorEnabled = false,
		.isNavigationEnabled = true,
	}));

	buttons3D_.push_back(uiWorld_.AddButton3D({
		.text = "3D Cursor + Navigation",
		.transform = {
			.position = { 0.0f, 0.5f, 0.0f },
			.size = { 3.8f, 0.7f },
		},
		.isNavigationEnabled = true,
	}));
	buttons3D_.push_back(uiWorld_.AddButton3D({
		.text = "3D Shortcut: 1 / Pad East",
		.transform = {
			.position = { 0.0f, -0.5f, 0.0f },
			.size = { 3.8f, 0.7f },
		},
		.shortcutCommand = "UI.Demo.Shortcut",
		.isNavigationEnabled = true,
	}));

	buttons_ = buttons2D_;
	buttons_.insert(buttons_.end(), buttons3D_.begin(), buttons3D_.end());
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

	UISelectionChange selectionChange = UISelectionChange::None;
	if (previousCommand_.Trigger()) {
		selectionChange = UISelectionChange::Previous;
	} else if (nextCommand_.Trigger()) {
		selectionChange = UISelectionChange::Next;
	}
	if (toggleHitboxesCommand_.Trigger()) {
		areHitboxesVisible_ = !areHitboxesVisible_;
	}

	uiWorld_.Update({
		.cursorPosition = cursorPosition,
		.isCursorPressed = pointerSubmitCommand_.Press(),
		.selectionChange = selectionChange,
		.isNavigationPressed = submitCommand_.Press(),
		.cursor3D = MakeUICursor3D(camera_, cursorPosition, 100.0f),
	});

	for (const UIButtonHandle button : buttons_) {
		if (uiWorld_.Trigger(button)) {
			message_ = uiWorld_.GetText(button) + " activated";
		}
	}
}

void UIWorldDemoScene::Draw() const {
	const Window::Size windowSize = Window::GetSize();

	camera_.SetScene();
	grid_.Draw();
	uiWorld_.Draw3D(buttonPanel_, buttonFont3D_);
	if (areHitboxesVisible_) {
		uiWorld_.DrawHitboxes3D(Color::Lime);
	}

	titleFont_.Draw(
		"UIWorld 2D / 3D Demo",
		{ static_cast<float>(windowSize.width) * 0.5f, 28.0f },
		{
			.color = Color::AliceBlue,
			.anchor = { 0.5f, 0.0f },
		});
	captionFont_.Draw(
		"W / S, Arrow Keys, D-Pad: Select    Enter / Space / Pad South: Submit",
		{ 24.0f, 86.0f },
		{ .color = Color::LightGray });
	captionFont_.Draw(
		"Mouse Left / Pad Right Shoulder: Pointer Submit    F / Pad North: Hitboxes",
		{ 24.0f, 112.0f },
		{ .color = Color::LightGray });
	captionFont_.Draw(
		"Escape: Return    1 / Pad East: 3D Button Shortcut",
		{ 24.0f, 138.0f },
		{ .color = Color::LightGray });
	captionFont_.Draw(
		"2D Buttons",
		{ 40.0f, 164.0f },
		{ .color = Color::Yellow });

	uiWorld_.Draw(buttonFont2D_);
	if (areHitboxesVisible_) {
		uiWorld_.DrawHitboxes({ 0.0f, 1.0f, 0.0f, 0.22f });
	}

	captionFont_.Draw(
		message_,
		{
			static_cast<float>(windowSize.width) * 0.5f,
			static_cast<float>(windowSize.height) - 64.0f,
		},
		{
			.color = Color::Yellow,
			.anchor = { 0.5f, 0.0f },
		});

	uiPointer_.Draw();
}
