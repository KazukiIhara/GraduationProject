#include "Boar.h"
#include "Collision/CollisionManager.h"
using namespace LGF;

void Boar::Init(const LGF::Vector3& spownPos) {
	position_ = spownPos;
	moveSpeed_ = 2.0f;
	hp_ = 700;
	color_ = Color::Purple;

	LGF::Collision::ColliderOptions bodyOptions{};
	bodyOptions.tag = "Boar_body";
	//bodyOptions.targetTags = { "","" };

	LGF::Collision::AABB bodyCollisionAABB{
		position_ + kColliderMin,
		position_ + kColliderMax
	};

	bodyColliderHandle_ = CollisionManager::GetInstance()->AddCollider(bodyCollisionAABB, bodyOptions);

	LGF::Collision::ColliderOptions attackAreaOptions{};
	attackAreaOptions.tag = "Boar_attackArea";
	//attackAreaOptions.targetTags = { "Player" };

	LGF::Collision::AABB attackAreaCollisionAABB{
		attackPoint_ + kColliderMin,
		attackPoint_ + kColliderMax,
	};


	attackAreaColliderHandle_ = CollisionManager::GetInstance()->AddCollider(attackAreaCollisionAABB, attackAreaOptions);

}

void Boar::Move() {
	//炒飯との位置関係
	LGF::Vector3 differenceCharHan = chaHanPosition_ - position_;//ターゲットと現在位置の差分ベクトル
	//進む
	position_ += LGF::Math::Normalize(differenceCharHan) * moveSpeed_ * (float)LGF::System::DeltaTime();//ベクトル正規化、スピードに合わせ移動
	//向き
	rotate_.y = std::atan2(differenceCharHan.x, differenceCharHan.z);
}

void Boar::Attack(Target target) {

	if(IsAnticipationTime(target, 2.0f) || isAttack_) {

		tackleTime_ += (float)LGF::System::DeltaTime();
		//進む
		position_ = LGF::Math::Lerp(tackleStart_,tackleGoal_, tackleTime_ / kTackleTimeMax_);//ベクトル正規化、スピードに合わせ移動
		attackPoint_ = position_;

		if (tackleTime_ >= kTackleTimeMax_) {
			tackleTime_ = 0.0f;
			AttackFinishCommon();
		}

	}
	else {
		//予備動作
		LGF::Vector3 difference;//ターゲットと現在位置の差分ベクトル
		//攻撃 突進
		switch (target)
		{
		case charhan:
			difference = chaHanPosition_ - position_;
			break;
		case player:
			difference = playerPosition_ - position_;
			break;
		default:
			break;
		}

		tackleStart_ = position_;
		tackleGoal_ = position_ + LGF::Math::Normalize(difference) * moveSpeed_ * 10.0f;
		tackleGoal_.y = 1.0f;
	}
}