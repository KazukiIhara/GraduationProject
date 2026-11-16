#pragma once

#include <string>

#include "LGF/LGFMath.h"
#include "LGF/Graphics/Model.h"

struct StageObjectData {
	std::string type; // オブジェクト種別
	LGF::Vector3 position; // 位置
	LGF::Vector3 rotation; // 回転
	LGF::Vector3 scale; // スケール
};

// =========================================================
// ステージのオブジェクト1つのクラス
// =========================================================
class StageObject {
public:
	StageObject(const std::string& modelPath, const StageObjectData& data);
	~StageObject();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	LGF::Model model_; // モデル
	LGF::Vector3 position_; // 位置
	LGF::Vector3 rotation_; // 回転
	LGF::Vector3 scale_; // スケール
};
