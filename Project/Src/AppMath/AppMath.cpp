#include "AppMath.h"

float AppMath::Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}

LGF::Vector3 AppMath::Lerp(LGF::Vector3 start, LGF::Vector3 end, float t) {
	LGF::Vector3 answer;
	answer.x = start.x + (end.x - start.x) * t;
	answer.y = start.y + (end.y - start.y) * t;
	answer.z = start.z + (end.z - start.z) * t;
	return answer;
}
