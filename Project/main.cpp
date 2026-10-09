#include <LGF/LGF.h>

#include "GameApp.h"
#include "Scene/MainScene.h"
#include "Scene/GamepadTestScene.h"
#include "Scene/AnimalTestScene.h"

using namespace LGF;

void Main() {
	GameApp sceneManager;
	sceneManager.Register<MainScene>(SceneNames::Main);
	sceneManager.Register<GamepadTestScene>(SceneNames::GamePadTest);
	sceneManager.Register<AnimalTestScene>(SceneNames::AnimalTest);
	sceneManager.Change(SceneNames::AnimalTest);

	while (System::Update()) {
		sceneManager.Update();
		sceneManager.Draw();
	}
}
