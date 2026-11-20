#pragma once

#include <LGF/LGF.h>

#include <string>

struct GameData {
	std::string previousScene;
};

using GameApp = LGF::SceneManager<std::string, GameData>;

namespace SceneNames {
	inline const std::string AnimalTest = "AnimalTest";
	inline const std::string GamePlayScene = "GamePlayScene";
	inline const std::string FeatureShowcase = "FeatureShowcase";

}
