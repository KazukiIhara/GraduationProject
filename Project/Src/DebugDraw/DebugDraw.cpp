#include "DebugDraw.h"

#include "LGF/Graphics/Line3D.h"
#include "LGF/LGFMath.h"
#include "LGF/Math/MathConstants.h"

void DebugDraw::DrawAABB(const LGF::Collision::AABB& box, const LGF::Vector4& color) {
	const auto& min = box.min;
	const auto& max = box.max;

	// AABBの8つの頂点を定義
	LGF::Vector3 p[8] = {
	    {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, min.y, max.z},
        {min.x, min.y, max.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z},
	};

	// ボックスを構成する12のエッジ
	int edges[12][2] = {
	    {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},

        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},

        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7}
    };

    // 単位行列
	LGF::Matrix4x4 identity = LGF::Math::Identity4x4();

    // 12本の線を生成して描画
	for (const auto& edge : edges) {
        // 始点と終点を指定
		LGF::Line3D line(p[edge[0]], p[edge[1]]);
        // 描画
		line.Draw(identity, color);
    }
}

void DebugDraw::DrawSphere(const LGF::Vector3& center, float radius, const LGF::Vector4& color, int segments) { 
    LGF::Matrix4x4 identity = LGF::Math::Identity4x4();
	float step = 2.0f * LGF::Math::PI / static_cast<float>(segments);

    for (int i = 0; i < segments; ++i) {
		float theta1 = i * step;
		float theta2 = (i + 1) * step;

        float c1 = std::cosf(theta1);
		float s1 = std::sinf(theta1);
		float c2 = std::cosf(theta2);
		float s2 = std::sinf(theta2);

        // XY平面の円
        {
			LGF::Vector3 p1(center.x + radius * c1, center.y + radius * s1, center.z);
			LGF::Vector3 p2(center.x + radius * c2, center.y + radius * s2, center.z);
			LGF::Line3D(p1, p2).Draw(identity, color);
        }

        // XZ平面の円
        {
			LGF::Vector3 p1(center.x + radius * c1, center.y, center.z + radius * s1);
			LGF::Vector3 p2(center.x + radius * c2, center.y, center.z + radius * s2);
			LGF::Line3D(p1, p2).Draw(identity, color);
        }

        // YZ平面の円
		{
			LGF::Vector3 p1(center.x, center.y + radius * c1, center.z + radius * s1);
			LGF::Vector3 p2(center.x, center.y + radius * c2, center.z + radius * s2);
			LGF::Line3D(p1, p2).Draw(identity, color);
		}
    }
}
