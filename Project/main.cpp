#include <LGF/LGF.h>

#include "GameApp.h"
#include "Scene/MainScene.h"
#include "Scene/GamepadTestScene.h"

using namespace LGF;

void Main() {
	GameApp sceneManager;
	sceneManager.Register<MainScene>(SceneNames::Main);
	sceneManager.Register<GamepadTestScene>(SceneNames::GamePadTest);
	sceneManager.Change(SceneNames::GamePadTest);

	while (System::Update()) {
		sceneManager.Update();
		sceneManager.Draw();
	}
}
