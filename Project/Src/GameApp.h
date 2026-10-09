#pragma once

#include <LGF/LGF.h>

#include <string>

struct GameData {};

using GameApp = LGF::SceneManager<std::string, GameData>;

namespace SceneNames {
	inline const std::string Main = "Main";
	inline const std::string GamePadTest = "GamePadTest";
	inline const std::string AnimalTest = "AnimalTest";
}
