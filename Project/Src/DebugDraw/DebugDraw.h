#pragma once

#include "LGF/Math/Vector4.h"
#include "LGF/Math/Matrix4x4.h"
#include "LGF/Collision/Shapes3D.h"

// =========================================================
// コライダー可視化用機能
// =========================================================
class DebugDraw {
public:
	/// <summary>
	/// AABBのワイヤーフレーム表示
	/// </summary>
	/// <param name="box"></param>
	/// <param name="color"></param>
	static void DrawAABB(const LGF::Collision::AABB& box, const LGF::Vector4& color);
};
