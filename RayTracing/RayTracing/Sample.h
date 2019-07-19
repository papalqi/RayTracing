#pragma once
#include"CoreMinimal.h"

static Vector importanceSampleUpperHemisphere(Vector& up, float n)
{
	float phi, theta;
	float r1 = (float)rand() / RAND_MAX, r2 = (float)rand() / RAND_MAX;

	phi = r1 * 2 * PI;
	theta = n < 0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (n + 1)));
	Vector sample(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));

	Vector front, right;

	if (fabs(up.X) > fabs(up.Y))
		front = Vector(up.Z, 0, -up.X);
	else
		front = Vector(0, -up.Z, up.Y);

	front.Normalize();
	right = (up^ front);

	oocd::Matrix mat4(right,up, front,0);
	return (mat4.TransformPosition(sample)).GetSafeNormal();
}

static bool russianRoulette(float probability, float& survival)
{
	survival = (float)rand() / RAND_MAX;
	if (survival > probability) return true;
	return false;
}
