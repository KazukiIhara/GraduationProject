#include "StageManager.h"

#include "Collision/CollisionManager.h"
#include "DebugDraw/DebugDraw.h"

StageManager::StageManager() { Initialize(); }

StageManager::~StageManager() {}

void StageManager::Update() {}

void StageManager::Initialize() {
#pragma region 中華レンジのコライダー設定
	LGF::Collision::ColliderOptions options{};
	options.tag = "ChineseRange";
	options.targetTags = {};

	LGF::Collision::AABB colliderAABB{
		positionChineseRange_ + kColliderChineseRangeMin, 
		positionChineseRange_ + kColliderChineseRangeMax
	};

	colliderHandleChineseRange_ = CollisionManager::GetInstance()->AddCollider(colliderAABB, options);
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
#pragma region 中華レンジのコライダーデバッグ描画
	auto stateOpt = CollisionManager::GetInstance()->GetColliderState(colliderHandleChineseRange_);
	if (stateOpt.has_value()) {
		if (std::holds_alternative<LGF::Collision::AABB>(stateOpt->shape)) {
			DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(stateOpt->shape), {0.0f, 1.0f, 0.0f, 1.0f});
		}
	}
#pragma endregion

#endif
}

void StageManager::DrawImGuiDebug() {}
