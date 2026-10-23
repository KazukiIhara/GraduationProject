#pragma once
#include "GameApp.h"
#include "BaseAnimalState.h"

enum Target {
	charhan,
	player
};

/// <summary>
/// 動物の基盤クラス
/// </summary>
class BaseAnimal {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="spownPos">召喚場所</param>
	virtual void Init(const LGF::Vector3& spownPos) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// Setter_炒飯がある中華鍋の位置
	/// </summary>
	/// <param name="playerPos">現在のプレイヤーの位置を設定</param>
	void SetPlayerPosition(const LGF::Vector3& playerPos);

	void SetCharHanPosition(const LGF::Vector3& playerPos);

	/// <summary>
	/// 消滅処理
	/// </summary>
	/// <returns>死んだアニメーションのMax値を超えた</returns>
	bool IsDelete() {
		return deadAnimationTime_ >= kDeadAnimationMaxTime_;
	}

	void Damage();


	/// <summary>
	/// 移動処理(炒飯の匂いにつられる)
	/// </summary>
	void Move();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack(Target target);

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
	/// フラグ_攻撃中であるか
	/// </summary>
	/// <returns></returns>
	bool IsAttacking() { return isAttack_; }

	bool IsDead() {
		return hp_ <= 0;
	}

protected:

	LGF::Box3D object_;

	LGF::Vector3 position_;
	LGF::Vector3 rotate_;

	float moveSpeed_ = 0.0f;

	LGF::Vector3 chaHanPosition_;//炒飯の位置
	LGF::Vector3 playerPosition_;//プレイヤーの位置

	int hp_ = 500;//体力

	float attackRadius_ = 3.0f;//範囲に入ったら攻撃する

	float deadAnimationTime_ = 0.0f;
	const float kDeadAnimationMaxTime_ = 2.0f;

private:

	std::unique_ptr<BaseAnimalState> state_;


	float p0 = 1.5f;
	float p1 = 1.0f + p0;
	float t = 0.0f;

	bool reverce = false;
	bool isAttack_ = false;
};

