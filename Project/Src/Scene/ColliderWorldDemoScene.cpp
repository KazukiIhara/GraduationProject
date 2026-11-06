#include "Scene/ColliderWorldDemoScene.h"

#include <algorithm>

#include <imgui.h>

using namespace LGF;

namespace {

	constexpr float kPlayerRadius = 0.65f;
	const Collision::CollisionTag kPlayerTag = "Player";
	const Collision::CollisionTag kSolidTag = "Solid";
	const Collision::CollisionTag kSensorTag = "Sensor";

	const Collision::AABB kBox{
		.min = { -1.25f, 0.0f, -1.25f },
		.max = { 1.25f, 2.5f, 1.25f },
	};

	const Collision::Capsule kSensor{
		.start = { 3.5f, 0.75f, 0.0f },
		.end = { 3.5f, 2.75f, 0.0f },
		.radius = 0.85f,
	};

	const Collision::Ray kRay{
		.origin = { -7.5f, 3.5f, -2.5f },
		.direction = { 1.0f, -0.22f, 0.22f },
	};

	void CopyEvents(
		std::span<const Collision::ColliderCollision> source,
		std::vector<Collision::ColliderCollision>& destination) {
		destination.assign(source.begin(), source.end());
	}

}

ColliderWorldDemoScene::ColliderWorldDemoScene(const InitData& init) :
	SampleScene(init) {
	Reset();
}

void ColliderWorldDemoScene::Update() {
	UpdateCamera();

	if (Key::L.Trigger() || Key::Escape.Trigger()) {
		ChangeScene(SceneNames::GamePlayScene);
		return;
	}
	if (Key::R.Trigger()) {
		Reset();
	}
	if (Key::Space.Trigger()) {
		detectSensor_ = !detectSensor_;
	}

	const float moveSpeed = 4.0f * static_cast<float>(System::DeltaTime());
	if (Key::A.Press()) {
		playerPosition_.x -= moveSpeed;
	}
	if (Key::D.Press()) {
		playerPosition_.x += moveSpeed;
	}
	if (Key::W.Press()) {
		playerPosition_.z += moveSpeed;
	}
	if (Key::S.Press()) {
		playerPosition_.z -= moveSpeed;
	}
	playerPosition_.x = std::clamp(playerPosition_.x, -7.0f, 7.0f);
	playerPosition_.z = std::clamp(playerPosition_.z, -4.0f, 4.0f);

	const Collision::CollisionTagList playerTargets = detectSensor_
		? Collision::CollisionTagList{ kSolidTag, kSensorTag }
		: Collision::CollisionTagList{ kSolidTag };
	colliderWorld_.SetTargetTags(playerCollider_, playerTargets);
	colliderWorld_.SetShape(playerCollider_, Collision::Sphere{
		.center = playerPosition_,
		.radius = kPlayerRadius,
	});
	colliderWorld_.Step();

	const Collision::ColliderEvents events = colliderWorld_.GetEvents(playerCollider_);
	CopyEvents(events.enter, enterEvents_);
	CopyEvents(events.stay, stayEvents_);
	CopyEvents(events.exit, exitEvents_);
	raycastHit_ = colliderWorld_.RaycastClosest(
		kRay,
		16.0f,
		{ kSolidTag, kSensorTag });

	DrawDebugWindow();
}

void ColliderWorldDemoScene::Draw() const {
	DrawStage();

	Sphere3D(kPlayerRadius).Draw(
		Math::MakeTranslateMatrix(playerPosition_),
		colliderWorld_.IsColliding(playerCollider_) ? Color::Tomato : Color::RoyalBlue);

	const Vector3 boxSize = kBox.max - kBox.min;
	const Vector3 boxCenter = (kBox.min + kBox.max) * 0.5f;
	Box3D(boxSize.x, boxSize.y, boxSize.z).Draw(
		Math::MakeTranslateMatrix(boxCenter),
		colliderWorld_.IsColliding(boxCollider_) ? Color::Orange : Color::Silver);

	const Vector3 sensorCenter = (kSensor.start + kSensor.end) * 0.5f;
	const float sensorHeight = Math::Distance(kSensor.start, kSensor.end);
	Capsule3D(kSensor.radius, sensorHeight).Draw(
		Math::MakeTranslateMatrix(sensorCenter),
		colliderWorld_.IsColliding(sensorCollider_) ? Color::Magenta : Color::Violet);

	const Vector3 rayDirection = Math::Normalize(kRay.direction);
	const float rayDistance = raycastHit_ ? raycastHit_->hit.distance : 16.0f;
	Line3D(kRay.origin, kRay.origin + rayDirection * rayDistance).Draw(
		Math::Identity4x4(),
		Color::Yellow);
	if (raycastHit_) {
		Sphere3D(0.12f).Draw(
			Math::MakeTranslateMatrix(raycastHit_->hit.point),
			Color::White);
		Line3D(
			raycastHit_->hit.point,
			raycastHit_->hit.point + raycastHit_->hit.normal * 0.75f).Draw(
			Math::Identity4x4(),
			Color::Lime);
	}
}

void ColliderWorldDemoScene::Reset() {
	colliderWorld_.Clear();
	colliderWorld_.RegisterTags({ kPlayerTag, kSolidTag, kSensorTag });
	enterEvents_.clear();
	stayEvents_.clear();
	exitEvents_.clear();
	playerPosition_ = { -5.0f, 1.0f, 0.0f };
	detectSensor_ = true;

	playerCollider_ = colliderWorld_.Add(
		Collision::Sphere{
			.center = playerPosition_,
			.radius = kPlayerRadius,
		},
		Collision::ColliderOptions{
			.tag = kPlayerTag,
			.targetTags = { kSolidTag, kSensorTag },
			.objectHandle = { 1u, 1u },
		});
	boxCollider_ = colliderWorld_.Add(
		kBox,
		Collision::ColliderOptions{
			.tag = kSolidTag,
			.targetTags = { kPlayerTag },
			.objectHandle = { 2u, 1u },
		});
	sensorCollider_ = colliderWorld_.Add(
		kSensor,
		Collision::ColliderOptions{
			.tag = kSensorTag,
			.targetTags = { kPlayerTag },
			.objectHandle = { 3u, 1u },
		});
	colliderWorld_.Step();
}

void ColliderWorldDemoScene::DrawDebugWindow() {
	ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(440.0f, 0.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("ColliderWorld Demo")) {
		ImGui::TextUnformatted("WASD: move sphere | Space: toggle Sensor target");
		ImGui::TextUnformatted("R: reset | L / Escape: return to GamePlayScene");
		ImGui::Checkbox("Detect Sensor tag", &detectSensor_);

		ImGui::SeparatorText("Collision events");
		ImGui::Text(
			"Enter: %zu  Stay: %zu  Exit: %zu",
			enterEvents_.size(),
			stayEvents_.size(),
			exitEvents_.size());
		const auto drawEvents = [](const char* phase, const auto& events) {
			for (const Collision::ColliderCollision& event : events) {
				ImGui::BulletText(
					"%s  tag=%s  object=%u:%u  depth=%.3f",
					phase,
					event.tag.c_str(),
					event.objectHandle.index,
					event.objectHandle.generation,
					event.contact.penetrationDepth);
			}
		};
		drawEvents("Enter", enterEvents_);
		drawEvents("Stay", stayEvents_);
		drawEvents("Exit", exitEvents_);

		ImGui::SeparatorText("Raycast");
		if (raycastHit_) {
			ImGui::Text(
				"Hit object: %u:%u  distance: %.3f",
				raycastHit_->objectHandle.index,
				raycastHit_->objectHandle.generation,
				raycastHit_->hit.distance);
		} else {
			ImGui::TextDisabled("No hit");
		}
		ImGui::TextDisabled("Blue/red sphere: Player tag");
		ImGui::TextDisabled("Gray/orange box: Solid tag");
		ImGui::TextDisabled("Purple capsule: Sensor tag");
	}
	ImGui::End();
}
