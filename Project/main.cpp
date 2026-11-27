#include <LGF/LGF.h>

#include "GameApp.h"
#include "Scene/MainScene.h"
#include "Scene/GamepadTestScene.h"
#include "Scene/AnimalTestScene.h"
#include "Scene/ColliderWorldDemoScene.h"
#include "Scene/PhysicsWorldDemoScene.h"
#include "Scene/UIWorldDemoScene.h"
#include "Scene/SkinningAnimationDemoScene.h"

#include "Scene/GamePlayScene.h"

using namespace LGF;

void Main() {
	Input::RegisterCommand("UI.Demo.Open", {
		Key::U,
	});
	Input::RegisterCommand("UI.Demo.Previous", {
		Key::W,
		Key::Up,
		Gamepad{ 0u }.Button(GamepadButtonCode::DPadUp),
	});
	Input::RegisterCommand("UI.Demo.Next", {
		Key::S,
		Key::Down,
		Gamepad{ 0u }.Button(GamepadButtonCode::DPadDown),
	});
	Input::RegisterCommand("UI.Demo.Submit", {
		Key::Enter,
		Key::Space,
		Gamepad{ 0u }.Button(GamepadButtonCode::South),
	});
	Input::RegisterCommand("UI.Demo.PointerSubmit", {
		Mouse::Left,
		Gamepad{ 0u }.Button(GamepadButtonCode::RightShoulder),
	});
	Input::RegisterCommand("UI.Demo.Shortcut", {
		Key::Num1,
		Gamepad{ 0u }.Button(GamepadButtonCode::East),
	});
	Input::RegisterCommand("UI.Demo.ToggleHitboxes", {
		Key::F,
		Gamepad{ 0u }.Button(GamepadButtonCode::North),
	});

	GameApp sceneManager;
	sceneManager.Register<MainScene>(SceneNames::Main);
	sceneManager.Register<GamepadTestScene>(SceneNames::GamePadTest);
	sceneManager.Register<AnimalTestScene>(SceneNames::AnimalTest);
	sceneManager.Register<GamePlayScene>(SceneNames::GamePlayScene);
	sceneManager.Register<ColliderWorldDemoScene>(SceneNames::ColliderWorldDemo);
	sceneManager.Register<PhysicsWorldDemoScene>(SceneNames::PhysicsWorldDemo);
	sceneManager.Register<UIWorldDemoScene>(SceneNames::UIWorldDemo);
	sceneManager.Register<SkinningAnimationDemoScene>(SceneNames::SkinningAnimationDemo);
	sceneManager.Change(SceneNames::GamePlayScene);

	while (System::Update()) {
		sceneManager.Update();
		sceneManager.Draw();
	}
}
