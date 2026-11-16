#pragma once
#include "LGF/LGFMath.h"

// =========================================================
// 数学関数
// =========================================================
class AppMath
{
public:
	/// <summary>
	/// 線形補間
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static float Lerp(float start, float end, float t);

	/// <summary>
	/// 線形保管(Vector3版)
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static LGF::Vector3 Lerp(LGF::Vector3 start, LGF::Vector3 end, float t);
};

