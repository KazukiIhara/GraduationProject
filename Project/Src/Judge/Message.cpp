#include "Message.h"

#include <imgui.h>

#include "LGF/LGFMath.h"

using namespace LGF;

Message::Message() {
	messageMaterial_.baseColorTexture = Texture("../Assets/Textures/MessageTexture.png");
}

void Message::Update(const LGF::DebugCamera3D& camera) {
	Updatebillboard(camera);
}

void Message::Draw() const {
	message_.Draw(
		Math::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f },
			Math::MakeQuaternion(rotation_), 
			position_
		),
		messageMaterial_
	);
}

void Message::DrawImGuiDebug() {
}

void Message::Updatebillboard(const LGF::DebugCamera3D& camera) {
	// カメラの位置とメッセージの位置から、カメラから見た方向を計算
	LGF::Vector3 direction = camera.translation - position_;
	// 方向ベクトルを正規化
	direction = LGF::Math::Normalize(direction);
	// メッセージの回転を計算（Y軸回転のみ）
	rotation_.y = std::atan2(direction.x, direction.z);
}