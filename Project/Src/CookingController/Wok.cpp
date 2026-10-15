#include "Wok.h"

#include "imgui.h"

using namespace LGF;

Wok::Wok() {
	position_ = kBasePosition;
	rotation_ = kBaseRotation;
}

void Wok::Update() {

}

void Wok::Draw() const {
	// モデル描画
	if (model_.IsValid()) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f },
			Math::MakeRotateAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.0f),
			position_
		);

		ModelDrawOptions options{};
		options.color = Color::White;

		model_.Draw(worldMatrix, options);
	}
}

void Wok::DrawImGuiDebug() {
#ifdef _DEBUG
	ImGui::Begin("Wok");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);

	ImGui::End();
#endif	
}