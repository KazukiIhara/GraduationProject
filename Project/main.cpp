#include <LGF/LGF.h>

#include "GameApp.h"
#include "Scene/MainScene.h"
#include "Scene/GamepadTestScene.h"

#include "Scene/GamePlayScene.h"

using namespace LGF;

void Main() {
	GameApp sceneManager;
	sceneManager.Register<MainScene>(SceneNames::Main);
	sceneManager.Register<GamepadTestScene>(SceneNames::GamePadTest);
	sceneManager.Register<GamePlayScene>(SceneNames::GamePlayScene);
	sceneManager.Change(SceneNames::GamePlayScene);

	while (System::Update()) {
		sceneManager.Update();
		sceneManager.Draw();
	}
}
