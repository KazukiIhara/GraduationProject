#pragma once

#include "Scene/SampleScene.h"

#include <optional>
#include <vector>

/// <summary>
/// ColliderWorldの衝突イベント、タグ、Raycastを確認するシーン。
/// </summary>
class ColliderWorldDemoScene final : public SampleScene {
public:
	explicit ColliderWorldDemoScene(const InitData& init);

	void Update() override;
	void Draw() const override;

private:
	void Reset();
	void DrawDebugWindow();

	LGF::Collision::ColliderWorld colliderWorld_;
	LGF::Collision::ColliderHandle playerCollider_{};
	LGF::Collision::ColliderHandle boxCollider_{};
	LGF::Collision::ColliderHandle sensorCollider_{};
	LGF::Vector3 playerPosition_{ -5.0f, 1.0f, 0.0f };
	std::vector<LGF::Collision::ColliderCollision> enterEvents_;
	std::vector<LGF::Collision::ColliderCollision> stayEvents_;
	std::vector<LGF::Collision::ColliderCollision> exitEvents_;
	std::optional<LGF::Collision::WorldRaycastHit> raycastHit_;
	bool detectSensor_ = true;
};
