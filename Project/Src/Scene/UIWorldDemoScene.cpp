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
	buttonFont2D_(kFontPath, 20u),
	bodyFont_(kFontPath, 20u),
	captionFont_(kFontPath, 16u),
	buttonFont3D_(kFontPath, 32u),
	light_({ 0.3f, -1.0f, 0.5f }, Color::White, 1.5f) {
	AddButton3D("Cursor + Navigation", 1.2f, true, true);
	AddButton3D("Cursor Only", 0.4f, true, false);
	AddButton3D("Navigation Only", -0.4f, false, true);
	AddButton3D(
		"Direct Input: 1 / Pad East",
		-1.2f,
		false,
		false,
		[this] {
			return directButtonCommand_.Press();
		});
	AddButton2D("2D Overlay Button", { 24.0f, 184.0f }, true, false);

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
		.cursor3D = MakeUICursor3D(camera_, cursorPosition_, 100.0f),
	});
}

void UIWorldDemoScene::Draw() const {
	const Window::Size windowSize = Window::GetSize();

	camera_.SetScene();
	light_.SetScene();
	grid_.Draw();
	uiWorld_.Draw3D();
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

UIButtonHandle UIWorldDemoScene::AddButton2D(
	std::string text,
	const Vector2& position,
	bool cursorEnabled,
	bool navigationEnabled,
	std::function<bool()> isDirectPressed) {
	UIButtonDesc desc{};
	desc.transform = {
		.position = position,
		.size = { 240.0f, 48.0f },
	};
	desc.callbacks = MakeCallbacks(text);
	desc.draw = [this, text](const UIButtonDrawContext& context) {
		const Vector4 color = context.state.isHeld
			? Color::HotPink
			: context.state.isHovered
				? Color::RoyalBlue
				: Vector4{ 0.05f, 0.08f, 0.16f, 0.94f };
		context.rect.Draw(color);
		buttonFont2D_.Draw(
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

UIButtonHandle UIWorldDemoScene::AddButton3D(
	std::string text,
	float positionY,
	bool cursorEnabled,
	bool navigationEnabled,
	std::function<bool()> isDirectPressed) {
	UIButtonDesc3D desc{};
	desc.transform = {
		.position = { 0.0f, positionY, 0.0f },
		.size = { 3.6f, 0.65f },
	};
	desc.callbacks = MakeCallbacks(text);
	desc.draw = [this, text](const UIButtonDrawContext3D& context) {
		Vector4 color{ 0.12f, 0.18f, 0.32f, 1.0f };
		if (context.state.isSelected) {
			color = { 0.95f, 0.42f, 0.04f, 1.0f };
		}
		if (context.state.isHovered) {
			color = { 0.05f, 0.42f, 1.0f, 1.0f };
		}
		if (context.state.isHeld) {
			color = { 1.0f, 0.08f, 0.48f, 1.0f };
		}

		if (context.state.isSelected || context.state.isHovered ||
			context.state.isHeld) {
			Vector4 outlineColor = context.state.isSelected
				? Color::Yellow
				: Color::Cyan;
			if (context.state.isHovered) {
				outlineColor = Color::Cyan;
			}
			if (context.state.isHeld) {
				outlineColor = Color::White;
			}

			const Matrix4x4 outlineMatrix = Math::MakeAffineMatrix(
				{ 1.04f, 1.18f, 1.0f },
				{},
				{ 0.0f, 0.0f, 0.01f }) * context.worldMatrix;
			buttonPanel_.Draw(outlineMatrix, outlineColor);
		}

		buttonPanel_.Draw(context.worldMatrix, color);
		buttonFont3D_.Draw(
			text,
			Math::Transform({ 0.0f, 0.0f, -0.01f }, context.worldMatrix),
			{
				.color = Color::White,
				.anchor = { 0.5f, 0.5f },
				.worldUnitsPerPixel = 0.008f,
				.mode = Font3DMode::World,
			});
	};
	desc.isDirectPressed = std::move(isDirectPressed);
	desc.isCursorEnabled = cursorEnabled;
	desc.isNavigationEnabled = navigationEnabled;
	return uiWorld_.AddButton3D(desc);
}

UIButtonCallbacks UIWorldDemoScene::MakeCallbacks(const std::string& text) {
	return {
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
}
