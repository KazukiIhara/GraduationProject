#include "CollisionManager.h"

#include "imgui.h"

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}

bool CollisionManager::RegisterTag(const LGF::Collision::CollisionTag& tag) { 
	return colliderWorld_.RegisterTag(tag); 
}

void CollisionManager::Clear() {
	colliderWorld_.Clear();
	debugColliderHandles_.clear();
}

LGF::Collision::ColliderHandle CollisionManager::AddCollider(const LGF::Collision::ColliderShape& shape, const LGF::Collision::ColliderOptions& options) {
	auto handle = colliderWorld_.Add(shape, options);
	// デバッグ用リストにも追加
	if (handle.IsValid()) {
		debugColliderHandles_.push_back(handle);
	}
	return handle;
}

bool CollisionManager::RemoveCollider(LGF::Collision::ColliderHandle handle) {
	bool success = colliderWorld_.Remove(handle);
	// デバッグ用リストからも削除
	if (success) {
		std::erase(debugColliderHandles_, handle);
	}
	return success;
}

bool CollisionManager::SetShape(LGF::Collision::ColliderHandle handle, const LGF::Collision::ColliderShape& shape) { return colliderWorld_.SetShape(handle, shape); }

bool CollisionManager::SetEnabled(LGF::Collision::ColliderHandle handle, bool enabled) { return colliderWorld_.SetEnabled(handle, enabled); }

std::optional<LGF::Collision::ColliderState> CollisionManager::GetColliderState(LGF::Collision::ColliderHandle handle) const { 
	return colliderWorld_.GetCollider(handle); 
}

void CollisionManager::Step() { colliderWorld_.Step(); }

LGF::Collision::ColliderEvents CollisionManager::GetEvents(LGF::Collision::ColliderHandle handle) { return colliderWorld_.GetEvents(handle); }

std::optional<LGF::Collision::WorldRaycastHit> CollisionManager::RaycastClosest(const LGF::Collision::Ray& ray, float maxDistance, const LGF::Collision::CollisionTagList& tags) const {
	return colliderWorld_.RaycastClosest(ray, maxDistance, tags);
}

void CollisionManager::DrawImGuiDebug() {
#ifdef _DEBUG
	if (ImGui::Begin("Collision Manager Debug")) {
		ImGui::Text("Active Colliders: %zu", debugColliderHandles_.size());
		ImGui::Separator();

		for (auto it = debugColliderHandles_.begin(); it != debugColliderHandles_.end();) {
			auto handle = *it;
			auto stateOpt = colliderWorld_.GetCollider(handle);

			// すでにワールド側から削除されている場合はリストから除外して次へ
			if (!stateOpt.has_value()) {
				it = debugColliderHandles_.erase(it);
				continue;
			}

			const auto& state = stateOpt.value();

			// ツリーノードのラベル作成
			std::string nodeLabel = "ID: " + std::to_string(handle.index) + " [Tag: " + state.tag + "]";

			if (ImGui::TreeNode(nodeLabel.c_str())) {
				ImGui::Text("Enabled: %s", state.enabled ? "True" : "False");
				ImGui::Text("Tag: %s", state.tag.c_str());

				// ターゲットタグの一覧表示
				std::string targetTagsStr;
				for (size_t i = 0; i < state.targetTags.size(); ++i) {
					targetTagsStr += state.targetTags[i];
					if (i + 1 < state.targetTags.size())
						targetTagsStr += ", ";
				}
				ImGui::Text("Target Tags: [%s]", targetTagsStr.c_str());

				ImGui::Separator();
				ImGui::Text("Shape Details:");

				std::visit(
				    [](const auto& shape) {
					    using T = std::decay_t<decltype(shape)>;
					    if constexpr (std::is_same_v<T, LGF::Collision::Sphere>) {
						    ImGui::Text("  Type: Sphere");
						    ImGui::Text("  Center: (%.2f, %.2f, %.2f)", shape.center.x, shape.center.y, shape.center.z);
						    ImGui::Text("  Radius: %.2f", shape.radius);
					    } else if constexpr (std::is_same_v<T, LGF::Collision::AABB>) {
						    ImGui::Text("  Type: AABB");
						    ImGui::Text("  Min: (%.2f, %.2f, %.2f)", shape.min.x, shape.min.y, shape.min.z);
						    ImGui::Text("  Max: (%.2f, %.2f, %.2f)", shape.max.x, shape.max.y, shape.max.z);
					    } else if constexpr (std::is_same_v<T, LGF::Collision::Capsule>) {
						    ImGui::Text("  Type: Capsule");
						    ImGui::Text("  Start:  (%.2f, %.2f, %.2f)", shape.start.x, shape.start.y, shape.start.z);
						    ImGui::Text("  End:    (%.2f, %.2f, %.2f)", shape.end.x, shape.end.y, shape.end.z);
						    ImGui::Text("  Radius: %.2f", shape.radius);
					    } else {
						    ImGui::Text("  Type: (Other / Unhandled Shape)");
					    }
				    },
				    state.shape);

				ImGui::TreePop();
			}

			++it;
		}
	}
	ImGui::End();
#endif
}
