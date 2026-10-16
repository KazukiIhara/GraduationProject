#include "AppMath.h"

float AppMath::Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}
