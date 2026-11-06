#include "StageManager.h"

#include "Collision/CollisionManager.h"
#include "DebugDraw/DebugDraw.h"

StageManager::StageManager() { Initialize(); }

StageManager::~StageManager() {}

void StageManager::Update() {}

void StageManager::Initialize() {
	// 中華レンジのコライダー設定
	LGF::Collision::ColliderOptions chineseRangeOptions{};
	chineseRangeOptions.tag = "Solid";
	chineseRangeOptions.targetTags = {};

	LGF::Collision::AABB chineseRangeAABB{
		positionChineseRange_ + kColliderChineseRangeMin, 
		positionChineseRange_ + kColliderChineseRangeMax
	};

	colliderHandleChineseRange_ = CollisionManager::GetInstance()->AddCollider(chineseRangeAABB, chineseRangeOptions);
	
	// インタラクト用センサーのコライダー設定
	LGF::Collision::ColliderOptions InteractionSensorOptions{};
	InteractionSensorOptions.tag = "InteractionSensor";
	InteractionSensorOptions.targetTags = {};

	LGF::Collision::AABB sensorAABB{
	    positionChineseRange_ + LGF::Vector3{-3.0f, -1.0f, -3.0f}, 
		positionChineseRange_ + LGF::Vector3{3.0f,  2.0f,  3.0f }
	};

	colliderHandleInteractionSensor_ = CollisionManager::GetInstance()->AddCollider(sensorAABB, InteractionSensorOptions);
#pragma endregion
}

void StageManager::Draw() {
	/* モデル描画 */

	// 中華レンジ
	if (modelChineseRange_.IsValid()) {
		// ワールド行列の作成（位置やスケール、回転を指定）
		LGF::Matrix4x4 worldMatrix = LGF::Math::MakeAffineMatrix(
		    {1.0f, 1.0f, 1.0f},                                       // スケール (X, Y, Z)
		    LGF::Math::MakeRotateAxisAngle({0.0f, 1.0f, 0.0f}, 0.0f), // 回転
		    positionChineseRange_                                     // 位置
		);

		// 描画オプション
		LGF::ModelDrawOptions options{};
		options.color = LGF::Color::White;

		// モデル自身のDraw関数を呼び出す
		modelChineseRange_.Draw(worldMatrix, options);
	}

#ifdef _DEBUG
	/* コライダーデバッグ描画 */

	// 中華レンジ
	auto ChineseRangeStateOpt = CollisionManager::GetInstance()->GetColliderState(colliderHandleChineseRange_);
	if (ChineseRangeStateOpt.has_value()) {
		if (std::holds_alternative<LGF::Collision::AABB>(ChineseRangeStateOpt->shape)) {
			DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(ChineseRangeStateOpt->shape), {0.0f, 1.0f, 0.0f, 1.0f});
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
