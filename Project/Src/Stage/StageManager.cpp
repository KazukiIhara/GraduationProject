#include "StageManager.h"

#include "DebugDraw/DebugDraw.h"

StageManager::StageManager() {}

StageManager::~StageManager() {}

void StageManager::Update() {}

void StageManager::Initialize(LGF::Collision::ColliderWorld* colliderWorld) {
	colliderWorld_ = colliderWorld;

	if (colliderWorld_) {
		// 中華レンジのコライダー設定
		LGF::Collision::AABB rangeBox{
		    {-1.0f, 0.0f, -1.0f},
            {1.0f,  2.0f, 1.0f }
        };
		LGF::Collision::ColliderOptions rangeOptions{};
		rangeOptions.layer = 1u;
		rangeColliderHandle_ = colliderWorld_->Add(rangeBox, rangeOptions);
	}
}

void StageManager::Draw() {
	/* モデル描画 */
	
	// 中華レンジ
	if (chineseRangeModel_.IsValid()) {
		// ワールド行列の作成（位置やスケール、回転を指定）
		LGF::Matrix4x4 worldMatrix = LGF::Math::MakeAffineMatrix(
		    {1.0f, 1.0f, 1.0f},                                  // スケール (X, Y, Z)
		    LGF::Math::MakeRotateAxisAngle({0.0f, 1.0f, 0.0f}, 0.0f), // 回転
		    {0.0f, 1.0f, 0.0f}                                   // 位置
		);

		// 描画オプション
		LGF::ModelDrawOptions options{};
		options.color = LGF::Color::White;

		// モデル自身のDraw関数を呼び出す
		chineseRangeModel_.Draw(worldMatrix, options);
	}

#ifdef _DEBUG
	/* コライダー描画 */

	// 中華レンジ
	if (colliderWorld_ && colliderWorld_->GetShape(rangeColliderHandle_)) {
		if (const auto* shape = colliderWorld_->GetShape(rangeColliderHandle_)) {
			if (std::holds_alternative<LGF::Collision::AABB>(*shape)) {
				DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(*shape), {1.0f, 1.0f, 1.0f, 1.0f});
			}	
		}
	}
#endif
}

void StageManager::DrawImGuiDebug() {

}
