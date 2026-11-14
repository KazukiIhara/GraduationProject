#pragma once

#include <LGF/LGF.h>

#include <string>

struct GameData {
	std::string previousScene;
};

using GameApp = LGF::SceneManager<std::string, GameData>;

namespace SceneNames {
	inline const std::string Main = "Main";
	inline const std::string GamePadTest = "GamePadTest";
	inline const std::string AnimalTest = "AnimalTest";
	inline const std::string GamePlayScene = "GamePlayScene";
	inline const std::string ColliderWorldDemo = "ColliderWorldDemo";
	inline const std::string PhysicsWorldDemo = "PhysicsWorldDemo";
	inline const std::string UIWorldDemo = "UIWorldDemo";
	inline const std::string SkinningAnimationDemo = "SkinningAnimationDemo";

}
