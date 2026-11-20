#include "StageObject.h"

// LGF
#include  "LGF/System.h"

// c++
#include <filesystem> 

StageObject::StageObject(const std::string& modelPath, const StageObjectData& data) : model_(modelPath), position_(data.position), rotation_(data.rotation), scale_(data.scale) {
	//アニメーションがあるならアニメーターに登録
	if (std::filesystem::path(model_.GetFilePath()).filename() == "tree_01.gltf") {
		animator_ = LGF::ModelAnimator(model_);
		animator_.Play("Idle");
	}
}

StageObject::~StageObject() {}

void StageObject::Update() {
	//アニメーターの更新
	if (animator_.IsValid()) {
		animator_.Update((float)LGF::System::DeltaTime());
	}
}

void StageObject::Draw() {
	if (model_.IsValid()) {
		LGF::Matrix4x4 worldMatrix = LGF::Math::MakeAffineMatrix(
			scale_,                               // スケール (X, Y, Z)
			LGF::Math::MakeQuaternion(rotation_), // 回転
			position_                             // 位置
		);

		// 描画オプション
		LGF::ModelDrawOptions options{};
		options.color = LGF::Color::White;

		// モデル自身のDraw関数を呼び出す
		if (animator_.IsValid()) {
			model_.Draw(worldMatrix, animator_, options);
		}
		else {
			model_.Draw(worldMatrix, options);
		}
	}
}