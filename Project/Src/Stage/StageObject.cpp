#include "StageObject.h"

StageObject::StageObject(const std::string& modelPath, const StageObjectData& data) : model_(modelPath), position_(data.position), rotation_(data.rotation), scale_(data.scale) {}

StageObject::~StageObject() {}

void StageObject::Update() {}

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
		model_.Draw(worldMatrix, options);
	}
}