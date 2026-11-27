#include "BaseAnimal.h"
#include "Animals/SetUp/DropFoodManager.h"
#include "DebugDraw/DebugDraw.h"
#include "Collision/CollisionManager.h"

#include "LGF/Parameter.h"

using namespace LGF;

BaseAnimal::~BaseAnimal() {}

void BaseAnimal::Init(const std::string& name, const LGF::Vector3& spownPos) {
	position_ = spownPos;

	ParameterDocument document;
	const ParameterIoResult result = document.Load("../Assets/Parameters/" + name + ".json");

	if (!result) {
		throw std::runtime_error(result.message);
	}

	object_ = LGF::Model{"../Assets/Models/" + *document.Get<std::string>("name")};
	animator_ = LGF::ModelAnimator(object_);
	animator_.Play("Walk");
	
	hp_ = *document.Get<int32_t>("hp");
	moveSpeed_ = *document.Get<float>("moveSpeed");


	LGF::Collision::ColliderOptions bodyOptions{};
	bodyOptions.tag = name + "_body";
	//bodyOptions.targetTags = { "","" };

	LGF::Collision::AABB bodyCollisionAABB{
		position_ + *document.Get<Vector3>("collision_body"),
		position_ + *document.Get<Vector3>("collision_body")
	};

	bodyColliderHandle_ = CollisionManager::GetInstance()->AddCollider(bodyCollisionAABB, bodyOptions);

	LGF::Collision::ColliderOptions attackAreaOptions{};
	attackAreaOptions.tag = name + "_attackArea";
	//attackAreaOptions.targetTags = { "Player" };

	LGF::Collision::AABB attackAreaCollisionAABB{
		attackPoint_ + *document.Get<Vector3>("collision_attack"),
		attackPoint_ + *document.Get<Vector3>("collision_attack")
	};


	attackAreaColliderHandle_ = CollisionManager::GetInstance()->AddCollider(attackAreaCollisionAABB, attackAreaOptions);
}

void BaseAnimal::Update() {

	/* 中華レンジのインタラクト用センサー判定 */
	auto events = CollisionManager::GetInstance()->GetEvents(bodyColliderHandle_);

	auto checkSensor = [&](const auto& eventList) {
		for (const auto& collision : eventList) {
			if (collision.tag == "") {
				//プレイヤーの攻撃でダメージ
			}
		}
	};

	LGF::Collision::AABB attackAreaCollisionAABB{
		attackPoint_ + kAttackColliderMin,
		attackPoint_ + kAttackColliderMax
	};
	LGF::Collision::AABB bodyCollisionAABB{
		position_ + kColliderMin,
		position_ + kColliderMax
	};

	CollisionManager::GetInstance()->SetShape(attackAreaColliderHandle_, attackAreaCollisionAABB);
	CollisionManager::GetInstance()->SetShape(bodyColliderHandle_, bodyCollisionAABB);

	if (!state_) {
		state_ = std::make_unique<MoveAnimalState>();
	}

	if (animator_.IsValid()) {
		animator_.Update((float)System::DeltaTime());
	}

	state_->SetBaseAnimal(this);
	state_->Update();

	state_ = state_->ChangeState();
}

void BaseAnimal::Draw() {
	Matrix4x4 worldMatrix = Math::MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, Math::MakeQuaternion(rotate_), position_);
	
	// 描画オプション
	LGF::ModelDrawOptions options{};
	options.color = LGF::Color::White;
	
	if (animator_.IsValid()) {
		object_.Draw(worldMatrix, animator_,options);
	}
	else {
		object_.Draw(worldMatrix, options);
	}

#ifdef _DEBUG
	// コライダーデバッグ描画
	auto stateOpt = CollisionManager::GetInstance()->GetColliderState(attackAreaColliderHandle_);
	if (stateOpt.has_value()) {
		if (std::holds_alternative<LGF::Collision::AABB>(stateOpt->shape)) {
			DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(stateOpt->shape), { 1.0f, 0.0f, 0.0f, 1.0f });
		}
	}
	// コライダーデバッグ描画
	stateOpt = CollisionManager::GetInstance()->GetColliderState(bodyColliderHandle_);
	if (stateOpt.has_value()) {
		if (std::holds_alternative<LGF::Collision::AABB>(stateOpt->shape)) {
			DebugDraw::DrawAABB(std::get<LGF::Collision::AABB>(stateOpt->shape), { 0.0f, 1.0f, 0.0f, 1.0f });
		}
	}
#endif

}

void BaseAnimal::SetPlayerPosition(const LGF::Vector3& playerPos) {
	playerPosition_ = playerPos;
}

void BaseAnimal::SetCharHanPosition(const LGF::Vector3& playerPos) {
	chaHanPosition_ = playerPos;
}

bool BaseAnimal::IsAnticipationTime(Target target, float maxTime) {	
	
	if (isAttack_) {
		anticipationTime_ = 0.0f;
		return false;
	}

	if (anticipationTime_ >= maxTime) {
		isAttack_ = true;
		return true;
	}
	else {
		anticipationTime_ += (float)LGF::System::DeltaTime();

		//ターゲットがプレイヤーなら方向を向く
		if (target == Target::player) {
			rotate_.y = std::atan2(LGF::Math::Normalize(playerPosition_ - position_).x, LGF::Math::Normalize(playerPosition_ - position_).z);
		}
	}

	return false;
}

void BaseAnimal::AttackFinishCommon(){
	attackPoint_ = AppMath::InvisiblePlace();
	isAttack_ = false;
	isAttackFinish_ = true;
}

void BaseAnimal::Dead() {
	if (rotate_.x >= -1.25f) {
		rotate_.x -= 1.25f * (float)LGF::System::DeltaTime();
	}
	deadAnimationTime_ += (float)LGF::System::DeltaTime();
	
	CollisionManager::GetInstance()->RemoveCollider(bodyColliderHandle_);	
	CollisionManager::GetInstance()->RemoveCollider(attackAreaColliderHandle_);

	if(IsDelete()) {
		DropFoodManager::GetInstance()->ActivationDropFood(position_);
	}
}


bool BaseAnimal::IsAttackAreaInTarget(Target target) {
	float dot = 0.0f;
	if (target == Target::charhan) {
		//炒飯との位置関係 //ターゲットと現在位置の差分ベクトル
		//炒飯とのベクトルの大きさ
		dot = LGF::Math::Length(chaHanPosition_ - position_);
	}
	else if (target == Target::player) {
		//プレイヤーとの位置関係 //ターゲットと現在位置の差分ベクトル	
		//炒飯とのベクトルの大きさ
		dot = LGF::Math::Length(playerPosition_ - position_);
	}

	//対象物との距離が一定以下なら進まない
	if (dot <= attackRadius_) {
		isAttackFinish_ = false;
		return true;
	}

	return false;
}
