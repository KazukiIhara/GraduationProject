#pragma once
#include <LGF/LGF.h>

///=====================================///
/// enums
///=====================================/// 

enum class JoyConType
{
	Left,
	Right
};

enum class DeadZoneType
{
	None,
	Small,
	Medium,
	Large
};

///=====================================///
/// structs
///=====================================/// 

/// <summary>
/// ジャイロの基準値を保持する構造体
/// </summary>
struct GyroReference
{
	LGF::Vector3 position = {};		// 座標
	LGF::Vector3 rotation = {};		// 回転
};




/// <summary>
/// ジョイコンのキャリブレーションを行うクラス
/// </summary>
class Calibration
{
public:
	///=====================================///
	/// public methods
	///=====================================/// 

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ジャイロの基準値をリセットする
	/// </summary>
	void ResetGyroReference() {
		gyroReference_ = {};
	}




	///=====================================///
	/// accessors
	///=====================================/// 

	/// <summary>
	/// 振る強度(縦軸限定)を取得する
	/// </summary>
	/// <param name="joyConType">ジョイコンの左右</param>
	bool GetShakeStrength(const JoyConType& joyConType, const DeadZoneType& deadZoneType);

private:
	///=====================================///
	/// private members
	///=====================================/// 

	//ジャイロの基準値(座標)
	LGF::Vector3 gyroReference_ = {};

	bool wasLeftConnected_ = false;
	bool wasRightConnected_ = false;

	//前フレームのY軸のアクセラレーション
	float previousYAcceleration_ = 0.0f;

};

