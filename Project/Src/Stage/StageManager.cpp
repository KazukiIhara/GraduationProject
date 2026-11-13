#include "StageManager.h"

#include "fstream"
#include "assert.h"

#include "../../Dependencies/LaziealGraphicsFramework/Project/Externals/nlohmann/json.hpp"

#include "Collision/CollisionManager.h"
#include "DebugDraw/DebugDraw.h"

StageManager::StageManager(const std::string& jsonFilePath) { Initialize(jsonFilePath); }

StageManager::~StageManager() {}

void StageManager::Update() {
	// ステージオブジェクトの更新
	for (auto& obj : objects_) {
		obj->Update();
	}
}

void StageManager::Initialize(const std::string& jsonFilePath) {
	// JSONファイルを開く
	std::ifstream file(jsonFilePath);
	if (!file.is_open()) {
		assert(false);
		return;
	}

	nlohmann::json jsonRoot;
	file >> jsonRoot;

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
