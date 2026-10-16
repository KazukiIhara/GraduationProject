#pragma once

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
};

