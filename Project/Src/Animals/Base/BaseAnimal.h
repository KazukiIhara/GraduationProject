#pragma once
#include "GameApp.h"
#include "BaseAnimalState.h"
#include "LGF/Collision/ColliderWorld.h"
#include "AppMath/AppMath.h"

enum Target {
	charhan,
	player
};

/// <summary>
/// 動物の基盤クラス
/// </summary>
class BaseAnimal {
public:
	
	virtual ~BaseAnimal();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">動物の名前</param>
	/// <param name="spownPos">召喚位置</param>
	void Init(const std::string& name,const LGF::Vector3& spownPos);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// Setter_プレイヤー位置
	/// </summary>
	/// <param name="playerPos">現在のプレイヤーの位置を設定</param>
	void SetPlayerPosition(const LGF::Vector3& playerPos);
	/// <summary>
	/// Setter_炒飯がある中華鍋の位置
	/// </summary>
	/// <param name="playerPos">中華鍋の位置を設定</param>
	void SetCharHanPosition(const LGF::Vector3& playerPos);

	/// <summary>
	/// 消滅処理
	/// </summary>
	/// <returns>死んだアニメーションのMax値を超えた</returns>
	bool IsDelete() const {
		return deadAnimationTime_ >= kDeadAnimationMaxTime_;
	}

	/// <summary>
	/// ダメージ (現在は即死)
	/// </summary>
	void Damage() {
		hp_ = 0;
	}

	/// <summary>
	/// 移動処理(炒飯の匂いにつられる)
	/// </summary>
	virtual void Move() = 0;

	/// <summary>
	/// 攻撃処理
	/// </summary>
	virtual void Attack(Target target) = 0;

	/// <summary>
	/// 死んだときの処理
	/// </summary>
	void Dead();

	/// <summary>
	/// フラグ_ターゲット(炒飯、プレイヤー)が攻撃範囲に入った場合
	/// </summary>
	/// <param name="target"></param>
	/// <returns></returns>
	bool IsAttackAreaInTarget(Target target);

	/// <summary>
	/// フラグ_攻撃を終えたか
	/// </summary>
	/// <returns></returns>
	bool IsAttackFinish() { return isAttackFinish_; }

	/// <summary>
	/// 死亡判定
	/// </summary>
	/// <returns>Hpが0の時、死亡状態である</returns>
	bool IsDead() {
		return hp_ <= 0;
	}

protected:
	/// <summary>
	/// 予備動作フラグ
	/// </summary>
	/// <returns></returns>
	bool IsAnticipationTime(Target target, float maxTime);

	/// <summary>
	/// 攻撃終了処理(共通分)
	/// </summary>
	void AttackFinishCommon();


	LGF::Model object_;
	LGF::ModelAnimator animator_;

	LGF::Vector3 position_;
	LGF::Vector3 rotate_;

	float moveSpeed_ = 0.0f;

	LGF::Vector3 chaHanPosition_;//炒飯の位置
	LGF::Vector3 playerPosition_;//プレイヤーの位置

	int32_t hp_ = 500;//体力

	float attackRadius_ = 3.0f;//範囲に入ったら攻撃する

	float deadAnimationTime_ = 0.0f;
	const float kDeadAnimationMaxTime_ = 2.0f;

	//当たり判定(body)
	const LGF::Vector3 kColliderMin = { -1.0f, -1.0f, -1.0f };
	const LGF::Vector3 kColliderMax = { 1.0f, 1.0f, 1.0f };
	
	const LGF::Vector3 kAttackColliderMin = { -1.0f, -0.5f, -1.0f };
	const LGF::Vector3 kAttackColliderMax = { 1.0f, 0.5f, 1.0f };

	LGF::Collision::ColliderHandle bodyColliderHandle_;// 動物自体
	LGF::Collision::ColliderHandle attackAreaColliderHandle_;// 攻撃範囲
	
	
	LGF::Vector3 attackPoint_ = AppMath::InvisiblePlace();

	bool isAttack_ = false;
	bool isAttackFinish_ = true;

private:

	std::unique_ptr<BaseAnimalState> state_;

	float anticipationTime_ = 0.0f;

};

