#pragma once
#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4804)
#include"../GCMath/Vector3.h"
#include "../GCMath/Vector2.h"
#include "../GCMath/MathHelper.h"
#include <vector>
#include <cstdlib>
#include "../GCMath/Matrix.h"
using namespace  std;
using namespace  oocd;
const float EPS = 0.001;
inline bool DoubleEquals(float a, float b, float eps = EPS)
{
	if (fabs(a - b) < eps) {
		return true;
	}
	return false;
}
inline int DoubleCompare(float a, float b, float eps = EPS)
{
	if (DoubleEquals(a, b, eps)) {
		return 0;
	}
	if (a < b) {
		return -1;
	}
	return 1;
}
#define Color Vector 