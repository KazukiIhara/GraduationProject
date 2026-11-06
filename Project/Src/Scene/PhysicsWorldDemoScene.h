#pragma once

#include "Scene/SampleScene.h"

#include <cstddef>
#include <cstdint>
#include <vector>

/// <summary>
/// PhysicsWorldの重力、静的OBBとの衝突、速度変更を確認するシーン。
/// </summary>
class PhysicsWorldDemoScene final : public SampleScene {
public:
	explicit PhysicsWorldDemoScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	void Reset();
	void SpawnSpheres(std::size_t count);
	void LaunchSpheres();
	void UpdatePlayer();
	void DrawDebugWindow();
	std::size_t GetFallingSphereCount() const;

	LGF::Physics::PhysicsWorld physicsWorld_;
	LGF::Physics::DynamicObjectHandle playerHandle_{};
	std::vector<LGF::Physics::StaticObjectState> staticObjectStates_;
	std::vector<LGF::Physics::DynamicObjectState> dynamicObjectStates_;
	uint64_t nextObjectId_ = 1u;
	float spawnTimer_ = 0.0f;
	bool autoSpawn_ = true;
	bool isPaused_ = false;
};
