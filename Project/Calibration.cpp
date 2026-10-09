#include "Calibration.h"

using namespace LGF;

void Calibration::Initialize()
{
}

void Calibration::Update()
{
}

bool Calibration::GetShakeStrength(const JoyConType& joyConType, const DeadZoneType& deadZoneType)
{
	bool result = false;

	// 静止時の重力加速度を定義
	constexpr float gravityAcceleration = 9.3f;

	// 現在のジョイコンの加速度を取得
	const std::optional<Gamepad> gamepad = (joyConType == JoyConType::Left) ? JoyCon::Left() : JoyCon::Right();



	return result;
}
