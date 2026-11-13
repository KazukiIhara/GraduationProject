#include "StageManager.h"

#include "fstream"
#include "assert.h"

#include "../../Dependencies/LaziealGraphicsFramework/Project/Externals/nlohmann/json.hpp"

#include "Collision/CollisionManager.h"
#include "DebugDraw/DebugDraw.h"

StageManager::StageManager(const std::string& jsonFilePath, LGF::Physics::PhysicsWorld& physicsWorld) : physicsWorld_(physicsWorld) { 
	Initialize(jsonFilePath, physicsWorld); 
}

StageManager::~StageManager() {}

void StageManager::Update() {
	// ステージオブジェクトの更新
	for (auto& obj : objects_) {
		obj->Update();
	}

	// 毎フレーム、静的オブジェクトの状態を取得（デバッグ用）
	physicsWorld_.GetStaticObjectStates(staticObjectStates_);
}

void StageManager::Initialize(const std::string& jsonFilePath, LGF::Physics::PhysicsWorld& physicsWorld) {
	// JSONファイルを開く
	std::ifstream file(jsonFilePath);
	if (!file.is_open()) {
		assert(false);
		return;
	}

	nlohmann::json jsonRoot;
	file >> jsonRoot;

	uint32_t objectIndex = 200u;

	// 各オブジェクトを生成
	if (jsonRoot.contains("objects") && jsonRoot["objects"].is_array()) {
		for (const auto& jsonObj : jsonRoot["objects"]) {
			StageObjectData data{};
			data.type = jsonObj["type"].get<std::string>();
			std::string modelName = jsonObj["name"].get<std::string>();


			// Position
			auto pos = jsonObj["position"];
			data.position = {pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>()};
			// Rotation
			auto rot = jsonObj["rotation"];
			data.rotation = {rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>()};
			// Scale
			auto scl = jsonObj["scale"];
			data.scale = {scl[0].get<float>(), scl[1].get<float>(), scl[2].get<float>()};
			// モデルパスの生成
			std::string modelPath = GetModelPath(data.type, modelName);
			// 生成してリストに追加
			objects_.push_back(std::make_unique<StageObject>(modelPath, data));


			// PhysicsWorldへの静的オブジェクト登録
			LGF::Vector3 baseHalfExtents{0.5f, 0.5f, 0.5f}; // デフォルト値
			float offsetY = 0.0f; // Y軸オフセット

			if (modelName == "ground") {
				baseHalfExtents = {100.0f, 0.5f, 100.0f};
				offsetY = -0.5f;
			} else if (modelName == "tree_01") {
				baseHalfExtents = {1.0f, 2.0f, 1.0f};
				offsetY = 2.0f;
			} else if (modelName == "tree_02") {
				baseHalfExtents = {1.0f, 2.0f, 1.0f};
				offsetY = 2.0f;
			} else if (modelName == "rock_01") {
				baseHalfExtents = {2.0f, 2.0f, 2.0f};
				offsetY = 1.0f;
			} else if (modelName == "tree_02") {
				baseHalfExtents = {1.0f, 2.0f, 1.0f};
				offsetY = 1.0f;
			} else if (modelName == "grass_01") {
				baseHalfExtents = {1.0f, 1.0f, 1.0f};
				offsetY = 1.0f;
			} else if (modelName == "chineseRange") {
				baseHalfExtents = {1.0f, 1.0f, 1.0f};
				offsetY = 1.0f;
			}

			LGF::Vector3 finalHalfExtents = {
				baseHalfExtents.x * data.scale.x, 
				baseHalfExtents.y * data.scale.y, 
				baseHalfExtents.z * data.scale.z
			};

			LGF::Vector3 adjustedPosition = {
				data.position.x, 
				data.position.y + offsetY * data.scale.y, 
				data.position.z
			};

			LGF::Collision::AABB boxShape {
				.min = adjustedPosition - finalHalfExtents, 
				.max = adjustedPosition + finalHalfExtents
            };

			LGF::Physics::StaticObjectOptions options{};
			options.material = {.restitution = 0.1f, .friction = 0.85f};
			options.objectHandle = {objectIndex++, 1u};

			// 物理ワールドに登録してハンドルを保持
			auto handle = physicsWorld.AddStaticObject(boxShape, options);
			staticPhysicsHandles_.push_back(handle);
		}
	}

	// インタラクト用センサーのコライダー設定
	LGF::Collision::ColliderOptions InteractionSensorOptions{};
	InteractionSensorOptions.tag = "InteractionSensor";
	InteractionSensorOptions.targetTags = {};

	LGF::Collision::AABB sensorAABB{
	    LGF::Vector3{-3.0f, -1.0f, -3.0f},
        LGF::Vector3{3.0f,  2.0f,  3.0f }
    };

	colliderHandleInteractionSensor_ = CollisionManager::GetInstance()->AddCollider(sensorAABB, InteractionSensorOptions);
}

void StageManager::Draw() {
	// ステージオブジェクトの描画
	for (auto& obj : objects_) {
		obj->Draw();
	}

#ifdef _DEBUG
	/* コライダーデバッグ描画 */

	// PhysicsWorldに登録されたオブジェクトのコライダー描画
	for (const auto& state : staticObjectStates_) {
		// 形状がAABBの場合
		if (const auto* aabb = std::get_if<LGF::Collision::AABB>(&state.shape)) {
			DebugDraw::DrawAABB(*aabb, {0.0f, 1.0f, 1.0f, 1.0f});
		}
	}

	// インタラクト用センサー
	auto InteractionSensorStateOpt = CollisionManager::GetInstance()->GetColliderState(colliderHandleInteractionSensor_);
	if (InteractionSensorStateOpt.has_value()) {
		if (std::holds_alternative<LGF::Collision::AABB>(InteractionSensorStateOpt->shape)) {
			DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(InteractionSensorStateOpt->shape), {1.0f, 0.0f, 1.0f, 1.0f});
		}
	}

#endif
}

void StageManager::DrawImGuiDebug() {}

std::string StageManager::GetModelPath(const std::string& type, const std::string& name) {
	std::string folder = "empty";

	if (type == "Environment") {
		folder = "environment";
	} else if (type == "Furniture") {
		folder = "furniture";
	}

	return "../Assets/Models/" + folder + "/" + name + "/" + name + ".gltf";
}
