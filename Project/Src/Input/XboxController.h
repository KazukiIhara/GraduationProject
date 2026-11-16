#pragma once

#include "LGF/Input.h"

#include <optional>

// =========================================================
// Xboxコントローラー取得処理
// =========================================================
namespace XboxController {

	/// <summary>
	/// 接続されているXbox系コントローラーを取得
	/// </summary>
	inline std::optional<LGF::Gamepad> Get() {
		for (uint32_t index = 0u; index < LGF::Gamepad::MaxCount; ++index) {
			const LGF::Gamepad gamepad{ index };
			if (!gamepad.IsConnected()) {
				continue;
			}

			const LGF::GamepadType type = gamepad.Type();
			if (type == LGF::GamepadType::Xbox360 ||
				type == LGF::GamepadType::XboxOne) {
				return gamepad;
			}
		}

		return std::nullopt;
	}

}
