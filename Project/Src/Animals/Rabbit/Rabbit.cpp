#include "Rabbit.h"
#include "AppMath/AppMath.h"
#include "Collision/CollisionManager.h"

using namespace LGF;

void Rabbit::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 2.0f;
	hp_ = 600;
	color_ = Color::Green;

	LGF::Collision::ColliderOptions bodyOptions{};
	bodyOptions.tag = "Rabbit_body";
	//bodyOptions.targetTags = { "","" };

	LGF::Collision::AABB bodyCollisionAABB{
		position_ + kColliderMin,
		position_ + kColliderMax
	};

	bodyColliderHandle_ = CollisionManager::GetInstance()->AddCollider(bodyCollisionAABB, bodyOptions);

	LGF::Collision::ColliderOptions attackAreaOptions{};
	attackAreaOptions.tag = "Rabbit_attackArea";
	//attackAreaOptions.targetTags = { "Player" };

	LGF::Collision::AABB attackAreaCollisionAABB{
		attackPoint_ + kColliderMin,
		attackPoint_ + kColliderMax,
	};

	
	attackAreaColliderHandle_ = CollisionManager::GetInstance()->AddCollider(attackAreaCollisionAABB, attackAreaOptions);
}

void Rabbit::Move() {
	jumpDelayTimer_ += (float)LGF::System::DeltaTime();
	if (jumpDelayTimer_ <= kJumpDelayTimeFinish_) {
		return;
	}

	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	
	if (jumpTimer_ <= 0.0f) {
		startPoint = position_;
		halfPoint = position_ + Math::Normalize(differenceCharHan);
		halfPoint.y = kJumpCurvePointY_;
		endPoint = position_ + (Math::Normalize(differenceCharHan) * moveSpeed_);
	}
	jumpTimer_ += (float)LGF::System::DeltaTime();

	Vector3 p0p1 = AppMath::Lerp(startPoint, halfPoint, jumpTimer_);
		
	Vector3 p1p2 = AppMath::Lerp(halfPoint, endPoint, jumpTimer_);
		
	//進む
	position_ = AppMath::Lerp(p0p1, p1p2, jumpTimer_);//ベクトル正規化、スピードに合わせ移動
	
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);

	if (jumpTimer_ >= kJumpTimeFinish_) {
		jumpTimer_ = 0.0f;
		jumpDelayTimer_ = 0.0f;
	}
}

void Rabbit::Attack(Target target) {
	if (IsAnticipationTime(target, 2.0f) || isAttack_) {
		LGF::Vector3 difference;
		switch (target)
		{
		case charhan:
			difference = chaHanPosition_ - position_;//炒飯を狙う
			break;
		case player:
			difference = playerPosition_ - position_;//プレイヤーを狙う
			break;
		default:
			break;
		}
		//攻撃 キック
		if (jumpTimer_ <= 0.0f) {
			startPoint = position_;
			halfPoint = position_ + Math::Normalize(difference);
			halfPoint.y = kJumpCurvePointY_;
			endPoint = position_ + (Math::Normalize(difference) * moveSpeed_);
		}

		jumpTimer_ += (float)LGF::System::DeltaTime();

		Vector3 p0p1 = AppMath::Lerp(startPoint, halfPoint, jumpTimer_);
		Vector3 p1p2 = AppMath::Lerp(halfPoint, endPoint, jumpTimer_);

		position_ = AppMath::Lerp(p0p1, p1p2, jumpTimer_);//ベクトル正規化、スピードに合わせ移動
		attackPoint_ = position_;

		if (jumpTimer_ >= kJumpTimeFinish_) {
			attackPoint_ = AppMath::InvisiblePlace();
			jumpTimer_ = 0.0f;
			jumpDelayTimer_ = 0.0f;
			isAttack_ = false;
			isAttackFinish_ = true;
		}
	}
	else {
		jumpTimer_ = 0.0f;
		position_.y = 1.0f;
	}
}