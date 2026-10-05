#include <LGF/LGF.h>

#include "GameApp.h"
#include "Scene/MainScene.h"

using namespace LGF;

void Main() {
	GameApp sceneManager;
	sceneManager.Register<MainScene>(SceneNames::Main);

	while (System::Update()) {
		sceneManager.Update();
		sceneManager.Draw();
	}
}
