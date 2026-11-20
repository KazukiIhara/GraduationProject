#include <LGF/LGF.h>

#include "GameApp.h"
#include "Scene/AnimalTestScene.h"
#include "Scene/FeatureShowcaseScene.h"
#include "Scene/GamePlayScene.h"

using namespace LGF;

void Main() {
	Input::RegisterCommand("Showcase.Open", {
		Key::U,
	});

	GameApp sceneManager;
	sceneManager.Register<AnimalTestScene>(SceneNames::AnimalTest);
	sceneManager.Register<GamePlayScene>(SceneNames::GamePlayScene);
	sceneManager.Register<FeatureShowcaseScene>(SceneNames::FeatureShowcase);
	sceneManager.Change(SceneNames::GamePlayScene);

	while (System::Update()) {
		sceneManager.Update();
		sceneManager.Draw();
	}
}
