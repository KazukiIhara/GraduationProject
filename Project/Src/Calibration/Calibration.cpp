#include "Calibration.h"

using namespace LGF;

bool Calibration::GetShakeStrength(const JoyConType& joyConType, const DeadZoneType& deadZoneType)
{
	bool result = false;

	// Joy-Conの接続状態を取得
	const std::optional<Gamepad> gamepad = (joyConType == JoyConType::Left) ? JoyCon::Left() : JoyCon::Right();
	if (!gamepad)
	{
		return result;
	}

	// 加速度を取得
	Vector3 acceleration = gamepad->Acceleration();

	// デッドゾーンの値を取得
	float deadZone = 0.0f;
	switch (deadZoneType)
	{
	case DeadZoneType::Small:
		deadZone = deadZoneSmall_;
		break;
	case DeadZoneType::Medium:
		deadZone = deadZoneMedium_;
		break;
	case DeadZoneType::Large:
		deadZone = deadZoneLarge_;
		break;
	default:
		break;
	}

	//重力の値
	float gravity = gravity_;

	// Y軸の加速度の変化量を計算（重力との差）
	float deltaYAcceleration = acceleration.y - gravity;

	if (std::abs(deltaYAcceleration) > deadZone)
	{
		result = true;
	}

	return result;
}
