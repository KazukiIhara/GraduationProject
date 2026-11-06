#include "Scene/PhysicsWorldDemoScene.h"

#include <algorithm>

#include <imgui.h>

using namespace LGF;

namespace {

	constexpr std::size_t kMaximumSphereCount = 40u;
	constexpr uint64_t kPlayerUserData = 0u;
	constexpr float kPlayerMoveSpeed = 4.5f;

	Physics::PhysicsWorldSettings MakeWorldSettings() {
		Physics::PhysicsWorldSettings settings{};
		settings.worldBounds = {
			.min = { -12.0f, -5.0f, -12.0f },
			.max = { 12.0f, 20.0f, 12.0f },
		};
		settings.substepCount = 8u;
		settings.solverIterationCount = 4u;
		return settings;
	}

	Collision::OBB MakeBox(
		const Vector3& center,
		const Vector3& halfExtents,
		float rotationZ = 0.0f) {
		return {
			.center = center,
			.halfExtents = halfExtents,
			.rotation = Math::MakeRotateAxisAngle({ 0.0f, 0.0f, 1.0f }, rotationZ),
		};
	}

	Matrix4x4 MakeBoxMatrix(const Collision::OBB& box) {
		return Math::MakeAffineMatrix(
			box.halfExtents * 2.0f,
			box.rotation,
			box.center);
	}

}

PhysicsWorldDemoScene::PhysicsWorldDemoScene(const InitData& init) :
	SampleScene(init),
	physicsWorld_(MakeWorldSettings()) {
	Reset();
}

void PhysicsWorldDemoScene::Update() {
	UpdateCamera();

	if (Key::P.Trigger() || Key::Escape.Trigger()) {
		ChangeScene(SceneNames::GamePlayScene);
		return;
	}
	if (Key::R.Trigger()) {
		Reset();
	}
	if (Key::Space.Trigger()) {
		SpawnSpheres(10u);
	}
	if (Key::I.Trigger()) {
		LaunchSpheres();
	}

	const float deltaTime = static_cast<float>(System::DeltaTime());
	if (!isPaused_) {
		UpdatePlayer();
		spawnTimer_ += deltaTime;
		if (autoSpawn_ &&
			spawnTimer_ >= 0.65f &&
			GetFallingSphereCount() < kMaximumSphereCount) {
			spawnTimer_ = 0.0f;
			SpawnSpheres(1u);
		}
		physicsWorld_.Update(deltaTime);
	}

	physicsWorld_.GetStaticObjectStates(staticObjectStates_);
	physicsWorld_.GetDynamicObjectStates(dynamicObjectStates_);
	DrawDebugWindow();
}

void PhysicsWorldDemoScene::Draw() const {
	DrawStage();

	for (const Physics::StaticObjectState& state : staticObjectStates_) {
		const Collision::OBB& box = std::get<Collision::OBB>(state.shape);
		Vector4 color = Color::Silver;
		if (state.userData == 1u) {
			color = Color::DarkGray;
		} else if (state.userData == 2u || state.userData == 3u) {
			color = Color::RoyalBlue;
		}
		Box3D(1.0f).Draw(MakeBoxMatrix(box), color);
	}

	for (const Physics::DynamicObjectState& state : dynamicObjectStates_) {
		const Collision::Sphere& sphere = std::get<Collision::Sphere>(state.shape);
		if (state.userData == kPlayerUserData) {
			Box3D(0.9f, 1.2f, 0.9f).Draw(
				Math::MakeTranslateMatrix(sphere.center),
				Color::SpringGreen);
			continue;
		}
		const Vector4 color = Math::Length(state.velocity) > 7.0f
			? Color::Tomato
			: Color::Gold;
		Sphere3D(sphere.radius).Draw(
			Math::MakeTranslateMatrix(sphere.center),
			color);
	}
}

void PhysicsWorldDemoScene::Reset() {
	physicsWorld_.Clear();
	physicsWorld_.SetSettings(MakeWorldSettings());
	staticObjectStates_.clear();
	dynamicObjectStates_.clear();
	nextObjectId_ = 1u;
	spawnTimer_ = 0.0f;
	autoSpawn_ = true;
	isPaused_ = false;

	physicsWorld_.AddStaticObject(
		MakeBox({ 0.0f, -0.5f, 0.0f }, { 7.0f, 0.5f, 6.0f }),
		true,
		1u);
	physicsWorld_.AddStaticObject(
		MakeBox(
			{ 2.3f, 6.1f, 1.5f },
			{ 3.7f, 0.18f, 1.6f },
			18.0f * Math::DEG_TO_RAD),
		true,
		2u);
	physicsWorld_.AddStaticObject(
		MakeBox(
			{ 1.7f, 3.6f, 1.5f },
			{ 3.7f, 0.18f, 1.6f },
			-18.0f * Math::DEG_TO_RAD),
		true,
		3u);
	physicsWorld_.AddStaticObject(
		MakeBox({ -7.25f, 2.0f, 0.0f }, { 0.25f, 2.0f, 6.0f }),
		true,
		4u);
	physicsWorld_.AddStaticObject(
		MakeBox({ 7.25f, 2.0f, 0.0f }, { 0.25f, 2.0f, 6.0f }),
		true,
		5u);
	physicsWorld_.AddStaticObject(
		MakeBox({ 0.0f, 2.0f, -6.25f }, { 7.0f, 2.0f, 0.25f }),
		true,
		6u);
	physicsWorld_.AddStaticObject(
		MakeBox({ 0.0f, 2.0f, 6.25f }, { 7.0f, 2.0f, 0.25f }),
		true,
		7u);

	playerHandle_ = physicsWorld_.AddDynamicObject(
		Collision::Sphere{
			.center = { 0.0f, 0.65f, -3.5f },
			.radius = 0.6f,
		},
		{},
		true,
		kPlayerUserData);

	SpawnSpheres(8u);
	physicsWorld_.GetStaticObjectStates(staticObjectStates_);
	physicsWorld_.GetDynamicObjectStates(dynamicObjectStates_);
}

void PhysicsWorldDemoScene::SpawnSpheres(std::size_t count) {
	const std::size_t available = kMaximumSphereCount -
		std::min(GetFallingSphereCount(), kMaximumSphereCount);
	const std::size_t spawnCount = std::min(count, available);

	for (std::size_t index = 0u; index < spawnCount; ++index) {
		const uint64_t objectId = nextObjectId_++;
		const int32_t horizontalOffset = static_cast<int32_t>(objectId % 3u) - 1;
		const int32_t depth = static_cast<int32_t>(objectId % 5u) - 2;
		const float radius = 0.28f + static_cast<float>(objectId % 4u) * 0.045f;

		physicsWorld_.AddDynamicObject(
			Collision::Sphere{
				.center = {
					5.2f + static_cast<float>(horizontalOffset) * 0.18f,
					8.2f + static_cast<float>(objectId % 8u) * 0.55f,
					1.5f + static_cast<float>(depth) * 0.45f,
				},
				.radius = radius,
			},
			{
				-0.2f,
				0.0f,
				0.0f,
			},
			true,
			objectId);
	}
}

void PhysicsWorldDemoScene::LaunchSpheres() {
	physicsWorld_.GetDynamicObjectStates(dynamicObjectStates_);
	for (const Physics::DynamicObjectState& state : dynamicObjectStates_) {
		if (state.userData == kPlayerUserData) {
			continue;
		}
		physicsWorld_.SetVelocity(
			state.handle,
			{ state.velocity.x, 7.5f, state.velocity.z });
	}
}

void PhysicsWorldDemoScene::UpdatePlayer() {
	const std::optional<Physics::DynamicObjectState> playerState =
		physicsWorld_.GetState(playerHandle_);
	if (!playerState) {
		return;
	}

	Vector3 movement{};
	if (Key::A.Press()) {
		movement.x -= 1.0f;
	}
	if (Key::D.Press()) {
		movement.x += 1.0f;
	}
	if (Key::W.Press()) {
		movement.z += 1.0f;
	}
	if (Key::S.Press()) {
		movement.z -= 1.0f;
	}
	if (Math::LengthSq(movement) > Math::EPSILON) {
		movement = Math::Normalize(movement) * kPlayerMoveSpeed;
	}

	physicsWorld_.SetVelocity(
		playerHandle_,
		{ movement.x, playerState->velocity.y, movement.z });
}

std::size_t PhysicsWorldDemoScene::GetFallingSphereCount() const {
	const std::size_t dynamicObjectCount = physicsWorld_.GetDynamicObjectCount();
	return physicsWorld_.IsValid(playerHandle_) && dynamicObjectCount > 0u
		? dynamicObjectCount - 1u
		: dynamicObjectCount;
}

void PhysicsWorldDemoScene::DrawDebugWindow() {
	ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(440.0f, 0.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("PhysicsWorld Demo")) {
		ImGui::TextUnformatted("WASD: move green player | Space: spawn 10 balls");
		ImGui::TextUnformatted("I: launch balls | R: reset");
		ImGui::TextUnformatted("P / Escape: return | Arrow keys/Q/E: camera");
		ImGui::Checkbox("Auto spawn", &autoSpawn_);
		ImGui::SameLine();
		ImGui::Checkbox("Paused", &isPaused_);

		if (ImGui::Button("Spawn 10")) {
			SpawnSpheres(10u);
		}
		ImGui::SameLine();
		if (ImGui::Button("Launch balls")) {
			LaunchSpheres();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			Reset();
		}

		const Physics::PhysicsWorldSettings& settings = physicsWorld_.GetSettings();
		ImGui::SeparatorText("World state");
		ImGui::Text(
			"Falling spheres: %zu / %zu",
			GetFallingSphereCount(),
			kMaximumSphereCount);
		ImGui::Text("Static OBBs: %zu", physicsWorld_.GetStaticObjectCount());
		ImGui::Text(
			"Gravity: (%.1f, %.1f, %.1f)",
			settings.gravity.x,
			settings.gravity.y,
			settings.gravity.z);
		ImGui::Text(
			"Substeps: %u  Solver iterations: %u",
			settings.substepCount,
			settings.solverIterationCount);
		ImGui::TextDisabled("Balls fall onto the upper slope, then roll down the lower slope.");
		ImGui::TextDisabled("The green player is rendered as a box with a Dynamic Sphere collider.");
		ImGui::TextDisabled("Dynamic objects do not collide with each other yet.");
	}
	ImGui::End();
}
